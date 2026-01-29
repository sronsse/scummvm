/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/endian.h"
#include "common/system.h"

#include "graphics/surface.h"

#include "scumm/editor/editor.h"
#include "scumm/editor/style.h"

namespace Scumm {

namespace Editor {

Charset::Charset(DCHR &dchr, LECF &lecf)
	: _dchr(dchr),
	  _lecf(lecf),
	  _selectedCharset(-1),
	  _selectedChar(-1),
	  _selectedPaletteRoom(-1),
	  _texture(nullptr) {
}

Charset::~Charset() {
	if (_texture)
		g_system->freeImGuiTexture(_texture);
}

LFLF *Charset::findLFLFByRoom(int roomNum) {
	if (roomNum <= 0)
		return nullptr;

	for (uint i = 0; i < _lecf.loff.locations.size(); ++i) {
		if (_lecf.loff.locations[i] == roomNum)
			return &_lecf.lflfs[i];
	}
	return nullptr;
}

CHR *Charset::findCharset(int charsetId) {
	if (charsetId < 0 || charsetId >= (int)_dchr.locations.size())
		return nullptr;

	LFLF *lflf = findLFLFByRoom(_dchr.locations[charsetId]);
	if (!lflf)
		return nullptr;

	for (uint k = 0; k < lflf->chars.size(); ++k) {
		if (lflf->chars[k].id == charsetId)
			return &lflf->chars[k];
	}
	return nullptr;
}

LFLF *Charset::findLFLF(int charsetId) {
	if (charsetId < 0 || charsetId >= (int)_dchr.locations.size())
		return nullptr;
	return findLFLFByRoom(_dchr.locations[charsetId]);
}

static int getCellWidth(const byte *data, uint32 dataSize, uint16 numChars) {
	uint32 offsetTableStart = 25;
	int cellWidth = 0;
	for (uint16 i = 0; i < numChars; ++i) {
		if (offsetTableStart + i * 4 + 4 > dataSize)
			break;
		uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + i * 4);
		uint32 absOffset = 21 + glyphOffset;
		if (absOffset + 4 > dataSize)
			continue;
		int w = data[absOffset];
		if (w > cellWidth)
			cellWidth = w;
	}
	return (cellWidth > 0) ? cellWidth : 8;
}

void *Charset::createCharsetTexture(CHR *chr, LFLF *lflf) {
	if (!chr || chr->data.size() < 25)
		return nullptr;
	if (!lflf || lflf->room.pals.wrap.apals.empty())
		return nullptr;

	const Common::Array<Color> &roomColors = lflf->room.pals.wrap.apals[0].colors;
	if (roomColors.empty())
		return nullptr;

	const byte *data = chr->data.data();
	uint32 dataSize = chr->data.size();

	// Parse charset header
	byte bpp = data[21];
	byte fontHeight = data[22];
	uint16 numChars = READ_LE_UINT16(data + 23);
	if (bpp == 0 || fontHeight == 0 || numChars == 0)
		return nullptr;

	// Build charset palette
	byte charPalette[16];
	charPalette[0] = 0;
	for (int i = 0; i < 15; ++i)
		charPalette[i + 1] = data[6 + i];

	// Compute layout
	int cols = 16;
	int rows = (numChars + cols - 1) / cols;
	int cellWidth = getCellWidth(data, dataSize, numChars);
	uint32 offsetTableStart = 25;
	int cellHeight = fontHeight;
	int imgWidth = cols * cellWidth;
	int imgHeight = rows * cellHeight;
	if (imgWidth == 0 || imgHeight == 0)
		return nullptr;

	// Create surface
	Graphics::Surface surface;
	surface.create(imgWidth, imgHeight, Graphics::PixelFormat::createFormatCLUT8());
	memset(surface.getPixels(), 0, surface.h * surface.pitch);

	// Render each glyph
	for (uint16 i = 0; i < numChars; ++i) {
		if (offsetTableStart + i * 4 + 4 > dataSize)
			break;
		uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + i * 4);
		uint32 absOffset = 21 + glyphOffset;
		if (absOffset + 4 > dataSize)
			continue;

		byte glyphWidth = data[absOffset];
		byte glyphHeight = data[absOffset + 1];
		if (glyphWidth == 0 || glyphHeight == 0)
			continue;

		int col = i % cols;
		int row = i / cols;
		int destX = col * cellWidth;
		int destY = row * cellHeight;

		const byte *bitmapData = data + absOffset + 4;
		uint32 bitmapEnd = dataSize - absOffset - 4;
		int bitPos = 0;
		int totalBits = glyphWidth * glyphHeight * bpp;
		if ((uint32)((totalBits + 7) / 8) > bitmapEnd)
			continue;

		for (int gy = 0; gy < glyphHeight; ++gy) {
			for (int gx = 0; gx < glyphWidth; ++gx) {
				byte pixel = 0;
				for (int b = 0; b < bpp; ++b) {
					int curByte = (bitPos + b) / 8;
					int curBit = 7 - ((bitPos + b) % 8);
					pixel = (pixel << 1) | ((bitmapData[curByte] >> curBit) & 1);
				}
				bitPos += bpp;

				if (pixel == 0)
					continue;

				int px = destX + gx;
				int py = destY + gy;
				if (px < imgWidth && py < imgHeight) {
					byte colorIndex = (pixel < 16) ? charPalette[pixel] : 0;
					*((byte *)surface.getBasePtr(px, py)) = colorIndex;
				}
			}
		}
	}

	// Build RGB palette
	byte palette[256 * 3];
	memset(palette, 0, sizeof(palette));
	for (uint i = 0; i < roomColors.size() && i < 256; ++i) {
		palette[i * 3 + 0] = roomColors[i].r;
		palette[i * 3 + 1] = roomColors[i].g;
		palette[i * 3 + 2] = roomColors[i].b;
	}

	void *texture = g_system->getImGuiTexture(surface, palette, 256);
	surface.free();
	return texture;
}

void Charset::drawCharsetPreview(ImDrawList *drawList, ImVec2 pos, int scale, CHR *chr, LFLF *lflf, bool transparent) {
	if (!chr || chr->data.size() < 25)
		return;
	if (!lflf || lflf->room.pals.wrap.apals.empty())
		return;

	const byte *data = chr->data.data();
	uint32 dataSize = chr->data.size();

	// Parse charset header
	byte fontHeight = data[22];
	uint16 numChars = READ_LE_UINT16(data + 23);
	if (fontHeight == 0 || numChars == 0)
		return;

	// Compute layout
	int cols = 16;
	int cellWidth = getCellWidth(data, dataSize, numChars);
	int cellHeight = fontHeight;

	// Render each glyph
	for (uint16 i = 0; i < numChars; ++i) {
		int col = i % cols;
		int row = i / cols;
		ImVec2 charPos(pos.x + col * cellWidth * scale, pos.y + row * cellHeight * scale);
		drawCharacter(drawList, charPos, scale, chr, lflf, i, transparent);
	}
}

void Charset::drawCharacter(ImDrawList *drawList, ImVec2 pos, int scale, CHR *chr, LFLF *lflf, int charIndex, bool transparent) {
	if (!chr || chr->data.size() < 25)
		return;
	if (!lflf || lflf->room.pals.wrap.apals.empty())
		return;

	const Common::Array<Color> &roomColors = lflf->room.pals.wrap.apals[0].colors;
	if (roomColors.empty())
		return;

	const byte *data = chr->data.data();
	uint32 dataSize = chr->data.size();

	// Parse charset header
	byte bpp = data[21];
	uint16 numChars = READ_LE_UINT16(data + 23);
	if (bpp == 0 || charIndex < 0 || charIndex >= numChars)
		return;

	// Build charset palette
	byte charPalette[16];
	charPalette[0] = 0;
	for (int i = 0; i < 15; ++i)
		charPalette[i + 1] = data[6 + i];

	// Get glyph data
	uint32 offsetTableStart = 25;
	if (offsetTableStart + charIndex * 4 + 4 > dataSize)
		return;

	uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + charIndex * 4);
	uint32 absOffset = 21 + glyphOffset;
	if (absOffset + 4 > dataSize)
		return;

	byte glyphWidth = data[absOffset];
	byte glyphHeight = data[absOffset + 1];
	if (glyphWidth == 0 || glyphHeight == 0)
		return;

	const byte *bitmapData = data + absOffset + 4;
	uint32 bitmapEnd = dataSize - absOffset - 4;
	int bitPos = 0;
	int totalBits = glyphWidth * glyphHeight * bpp;
	if ((uint32)((totalBits + 7) / 8) > bitmapEnd)
		return;

	// Render glyph
	for (int gy = 0; gy < glyphHeight; ++gy) {
		for (int gx = 0; gx < glyphWidth; ++gx) {
			byte pixel = 0;
			for (int b = 0; b < bpp; ++b) {
				int curByte = (bitPos + b) / 8;
				int curBit = 7 - ((bitPos + b) % 8);
				pixel = (pixel << 1) | ((bitmapData[curByte] >> curBit) & 1);
			}
			bitPos += bpp;

			if (transparent && pixel == 0)
				continue;

			byte colorIndex = (pixel < 16) ? charPalette[pixel] : 0;

			ImU32 pixelColor = IM_COL32(128, 128, 128, 255);
			if (colorIndex < roomColors.size()) {
				const Color &c = roomColors[colorIndex];
				pixelColor = IM_COL32(c.r, c.g, c.b, 255);
			}

			ImVec2 p1(pos.x + gx * scale, pos.y + gy * scale);
			ImVec2 p2(p1.x + scale, p1.y + scale);
			drawList->AddRectFilled(p1, p2, pixelColor);
		}
	}
}

void Charset::renderCharacterTooltip(CHR *chr, LFLF *lflf, int charIndex) {
	if (!chr || chr->data.size() < 25)
		return;
	if (!lflf || lflf->room.pals.wrap.apals.empty())
		return;

	const byte *data = chr->data.data();
	uint32 dataSize = chr->data.size();
	uint16 numChars = READ_LE_UINT16(data + 23);
	uint32 offsetTableStart = 25;

	if (charIndex < 0 || charIndex >= numChars)
		return;
	if (offsetTableStart + charIndex * 4 + 4 > dataSize)
		return;

	uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + charIndex * 4);
	uint32 absOffset = 21 + glyphOffset;
	if (absOffset + 4 > dataSize)
		return;

	byte glyphWidth = data[absOffset];
	byte glyphHeight = data[absOffset + 1];
	if (glyphWidth == 0 || glyphHeight == 0)
		return;

	ImGui::BeginTooltip();
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	int scale = 2;
	drawCharacter(drawList, canvasPos, scale, chr, lflf, charIndex, true);
	ImGui::Dummy(ImVec2((float)(glyphWidth * scale), (float)(glyphHeight * scale)));
	ImGui::EndTooltip();
}

void Charset::renderCharacterPreview(CHR *chr, LFLF *lflf, int charIndex) {
	if (!chr || chr->data.size() < 25)
		return;

	const byte *data = chr->data.data();
	uint32 dataSize = chr->data.size();
	uint16 numChars = READ_LE_UINT16(data + 23);
	uint32 offsetTableStart = 25;

	if (charIndex < 0 || charIndex >= numChars)
		return;
	if (offsetTableStart + charIndex * 4 + 4 > dataSize)
		return;

	uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + charIndex * 4);
	uint32 absOffset = 21 + glyphOffset;
	if (absOffset + 4 > dataSize)
		return;

	byte glyphWidth = data[absOffset];
	byte glyphHeight = data[absOffset + 1];
	if (glyphWidth == 0 || glyphHeight == 0)
		return;

	ImGui::SeparatorText("Character Preview");

	// Controls
	static int charPreviewScale = 2;
	static bool charPreviewTransparent = true;
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("Scale##charpreview", &charPreviewScale, 1, 8, "%dx");
	ImGui::SameLine();
	ImGui::Checkbox("Transparent##charpreview", &charPreviewTransparent);

	// Render
	ImGui::BeginChild("CharPreviewScroll", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	int scale = charPreviewScale;
	drawCharacter(drawList, canvasPos, scale, chr, lflf, charIndex, charPreviewTransparent);
	ImGui::Dummy(ImVec2((float)(glyphWidth * scale), (float)(glyphHeight * scale)));
	ImGui::EndChild();
}

void Charset::renderCharsetTooltip(int charsetId) {
	CHR *chr = findCharset(charsetId);
	if (!chr || chr->data.size() < 25)
		return;

	LFLF *lflf = findLFLF(charsetId);
	if (!lflf || lflf->room.pals.wrap.apals.empty())
		return;

	const byte *data = chr->data.data();
	uint32 dataSize = chr->data.size();
	byte fontHeight = data[22];
	uint16 numChars = READ_LE_UINT16(data + 23);
	if (fontHeight == 0 || numChars == 0)
		return;

	// Compute layout
	int cols = 16;
	int cellWidth = getCellWidth(data, dataSize, numChars);
	int rows = (numChars + cols - 1) / cols;
	int imgWidth = cols * cellWidth;
	int imgHeight = rows * fontHeight;
	if (imgWidth == 0 || imgHeight == 0)
		return;

	ImGui::BeginTooltip();
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	drawCharsetPreview(drawList, canvasPos, 1, chr, lflf, true);
	ImGui::Dummy(ImVec2((float)imgWidth, (float)imgHeight));
	ImGui::EndTooltip();
}

void Charset::renderCharsetList() {
	ImGui::BeginChild("CharsetList", ImVec2(200, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Charsets");

	for (uint i = 0; i < _dchr.locations.size(); ++i) {
		if (_dchr.locations[i] == 0)
			continue;

		int charsetId = (int)i;
		char label[64];
		Common::sprintf_s(label, "Charset %d", charsetId);

		if (ImGui::Selectable(label, _selectedCharset == charsetId)) {
			if (_selectedCharset != charsetId) {
				_selectedChar = -1;
				_selectedPaletteRoom = -1;
			}
			_selectedCharset = charsetId;
		}

		if (ImGui::IsItemHovered())
			renderCharsetTooltip(charsetId);
	}

	ImGui::EndChild();
}

void Charset::renderCharsetProperties() {
	ImGui::BeginChild("CharsetProperties", ImVec2(400, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

	if (_selectedCharset < 0) {
		ImGui::TextColored(SOL_BASE01, "Select a charset.");
		ImGui::EndChild();
		return;
	}

	CHR *chr = findCharset(_selectedCharset);
	if (!chr || chr->data.size() < 25) {
		ImGui::TextColored(SOL_BASE01, "Charset data not found.");
		ImGui::EndChild();
		return;
	}

	LFLF *lflf = findLFLF(_selectedCharset);

	const byte *data = chr->data.data();
	byte bpp = data[21];
	byte fontHeight = data[22];
	uint16 numChars = READ_LE_UINT16(data + 23);

	// Header
	char headerBuf[64];
	Common::sprintf_s(headerBuf, "Charset %d", _selectedCharset);
	ImGui::SeparatorText(headerBuf);

	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("CharsetHeader", 2, flags)) {
		ImGui::TableSetupColumn("Property");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "BPP");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", bpp);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Font Height");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", fontHeight);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Characters");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", numChars);

		ImGui::EndTable();
	}

	// Get palette from selected room
	LFLF *paletteLflf = (_selectedPaletteRoom < 0) ? lflf : findLFLFByRoom(_selectedPaletteRoom);

	if (ImGui::BeginTabBar("CharsetPropsTabs")) {
		if (ImGui::BeginTabItem("Palette")) {
			renderPalette(chr, lflf);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Characters")) {
			renderCharacters(chr, paletteLflf);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::EndChild();
}

void Charset::renderPalette(CHR *chr, LFLF *lflf) {
	byte charsetRoom = _dchr.locations[_selectedCharset];

	// Room selection combo
	ImGui::SeparatorText("Room Palette Source");

	int currentRoom = (_selectedPaletteRoom < 0) ? charsetRoom : _selectedPaletteRoom;
	char comboLabel[64];
	Common::sprintf_s(comboLabel, "Room %d%s", currentRoom, (_selectedPaletteRoom < 0) ? " (charset's room)" : "");

	if (ImGui::BeginCombo("Room", comboLabel)) {
		char defaultLabel[64];
		Common::sprintf_s(defaultLabel, "Room %d (charset's room)", charsetRoom);
		if (ImGui::Selectable(defaultLabel, _selectedPaletteRoom < 0))
			_selectedPaletteRoom = -1;

		for (uint i = 0; i < _lecf.loff.locations.size(); ++i) {
			if (_lecf.loff.locations[i] == 0)
				continue;
			if (_lecf.lflfs[i].room.pals.wrap.apals.empty())
				continue;
			int roomNum = _lecf.loff.locations[i];
			if (roomNum == charsetRoom)
				continue;
			char label[64];
			Common::sprintf_s(label, "Room %d", roomNum);
			if (ImGui::Selectable(label, _selectedPaletteRoom == roomNum))
				_selectedPaletteRoom = roomNum;
		}
		ImGui::EndCombo();
	}

	// Get palette
	LFLF *paletteLflf = (_selectedPaletteRoom < 0) ? lflf : findLFLFByRoom(_selectedPaletteRoom);
	if (!paletteLflf || paletteLflf->room.pals.wrap.apals.empty()) {
		ImGui::TextColored(SOL_RED, "Room palette not found.");
		return;
	}

	const Common::Array<Color> &roomColors = paletteLflf->room.pals.wrap.apals[0].colors;
	const byte *data = chr->data.data();

	// Display palette
	ImGui::SeparatorText("Charset Palette");
	ImGui::TextColored(SOL_BASE01, "(16 entries, mapped to room palette)");

	ImGui::BeginChild("CharsetPaletteGrid", ImVec2(0, 0), ImGuiChildFlags_Borders);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	for (int i = 0; i < 16; ++i) {
		ImGui::PushID(i);

		byte colorIndex = (i == 0) ? 0 : data[6 + (i - 1)];
		float col[3] = {0, 0, 0};
		if (colorIndex < roomColors.size()) {
			col[0] = roomColors[colorIndex].r / 255.0f;
			col[1] = roomColors[colorIndex].g / 255.0f;
			col[2] = roomColors[colorIndex].b / 255.0f;
		}
		ImGui::ColorEdit3("##color", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker);
		ImGui::SameLine();
		ImGui::TextColored(SOL_BASE0, "Index %d -> Palette %d", i, colorIndex);

		ImGui::PopID();
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void Charset::renderCharacters(CHR *chr, LFLF *lflf) {
	const byte *data = chr->data.data();
	uint32 dataSize = chr->data.size();
	uint16 numChars = READ_LE_UINT16(data + 23);
	uint32 offsetTableStart = 25;

	// Character list
	ImGui::BeginChild("CharList", ImVec2(100, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Characters");

	for (uint16 i = 0; i < numChars; ++i) {
		if (offsetTableStart + i * 4 + 4 > dataSize)
			break;
		uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + i * 4);
		uint32 absOffset = 21 + glyphOffset;
		if (absOffset + 4 > dataSize)
			continue;

		byte glyphWidth = data[absOffset];
		byte glyphHeight = data[absOffset + 1];
		if (glyphWidth == 0 && glyphHeight == 0)
			continue;

		char label[64];
		if (i >= 32 && i < 127)
			Common::sprintf_s(label, "'%c'##char%d", (char)i, i);
		else
			Common::sprintf_s(label, "Char %d", i);

		if (ImGui::Selectable(label, _selectedChar == (int)i))
			_selectedChar = (int)i;

		if (ImGui::IsItemHovered())
			renderCharacterTooltip(chr, lflf, i);
	}

	ImGui::EndChild();

	ImGui::SameLine();

	// Character properties
	ImGui::BeginChild("CharProperties", ImVec2(0, 0), ImGuiChildFlags_Borders);

	if (_selectedChar >= 0 && _selectedChar < numChars) {
		if (offsetTableStart + _selectedChar * 4 + 4 <= dataSize) {
			uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + _selectedChar * 4);
			uint32 absOffset = 21 + glyphOffset;

			if (absOffset + 4 <= dataSize) {
				byte glyphWidth = data[absOffset];
				byte glyphHeight = data[absOffset + 1];
				int8 xoff = (int8)data[absOffset + 2];
				int8 yoff = (int8)data[absOffset + 3];

				char headerBuf[64];
				Common::sprintf_s(headerBuf, "Character %d Properties", _selectedChar);
				ImGui::SeparatorText(headerBuf);

				const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
				if (ImGui::BeginTable("CharProps", 2, flags)) {
					ImGui::TableSetupColumn("Property");
					ImGui::TableSetupColumn("Value");
					ImGui::TableHeadersRow();

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BLUE, "Index");
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BASE0, "%d", _selectedChar);

					if (_selectedChar >= 32 && _selectedChar < 127) {
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::TextColored(SOL_BLUE, "Character");
						ImGui::TableNextColumn();
						ImGui::TextColored(SOL_BASE0, "%c", (char)_selectedChar);
					}

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BLUE, "Width");
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BASE0, "%d", glyphWidth);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BLUE, "Height");
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BASE0, "%d", glyphHeight);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BLUE, "X Offset");
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BASE0, "%d", xoff);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BLUE, "Y Offset");
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BASE0, "%d", yoff);

					ImGui::EndTable();
				}

				renderCharacterPreview(chr, lflf, _selectedChar);
			}
		}
	} else {
		ImGui::TextColored(SOL_BASE01, "Select a character.");
	}

	ImGui::EndChild();
}

void Charset::renderCharsetImage(CHR *chr, LFLF *lflf) {
	ImGui::BeginChild("CharsetImage", ImVec2(0, 0), ImGuiChildFlags_Borders);

	ImGui::SeparatorText("Charset Preview");

	if (!chr || chr->data.size() < 25) {
		ImGui::EndChild();
		return;
	}

	// Controls
	static int charsetScale = 2;
	static bool charsetBounds = true;
	static bool charsetTransparent = true;
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("Scale", &charsetScale, 1, 8, "%dx");
	ImGui::SameLine();
	ImGui::Checkbox("Bounds", &charsetBounds);
	ImGui::SameLine();
	ImGui::Checkbox("Transparent", &charsetTransparent);

	const byte *data = chr->data.data();
	uint32 dataSize = chr->data.size();
	byte fontHeight = data[22];
	uint16 numChars = READ_LE_UINT16(data + 23);
	uint32 offsetTableStart = 25;

	// Compute cell width
	int cols = 16;
	int cellWidth = getCellWidth(data, dataSize, numChars);
	int cellHeight = fontHeight;

	int rows = (numChars + cols - 1) / cols;
	int imgW = cols * cellWidth;
	int imgH = rows * cellHeight;

	int scale = charsetScale;

	ImGui::BeginChild("CharsetImageScroll", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	ImVec2 imageSize((float)(imgW * scale), (float)(imgH * scale));
	ImDrawList *drawList = ImGui::GetWindowDrawList();

	// Render charset
	if (charsetTransparent) {
		drawCharsetPreview(drawList, imagePos, scale, chr, lflf, true);
		ImGui::Dummy(imageSize);
	} else {
		if (_texture)
			g_system->freeImGuiTexture(_texture);
		_texture = createCharsetTexture(chr, lflf);
		if (_texture) {
			ImGui::Image((ImTextureID)(intptr_t)_texture, imageSize);
		} else {
			ImGui::Dummy(imageSize);
		}
	}

	// Draw selected character highlight
	if (_selectedChar >= 0 && _selectedChar < numChars) {
		if (offsetTableStart + _selectedChar * 4 + 4 <= dataSize) {
			uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + _selectedChar * 4);
			uint32 absOffset = 21 + glyphOffset;
			if (absOffset + 4 <= dataSize) {
				byte glyphWidth = data[absOffset];
				byte glyphHeight = data[absOffset + 1];
				if (glyphWidth > 0 && glyphHeight > 0) {
					int col = _selectedChar % cols;
					int row = _selectedChar / cols;
					ImVec2 rectMin(imagePos.x + col * cellWidth * scale, imagePos.y + row * cellHeight * scale);
					ImVec2 rectMax(rectMin.x + glyphWidth * scale, rectMin.y + glyphHeight * scale);
					ImU32 fillColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 0.75f));
					drawList->AddRectFilled(rectMin, rectMax, fillColor);
				}
			}
		}
	}

	// Draw bounding boxes
	if (charsetBounds) {
		ImU32 outlineColor = IM_COL32(255, 255, 0, 255);
		for (uint16 i = 0; i < numChars; ++i) {
			if (offsetTableStart + i * 4 + 4 > dataSize)
				break;
			uint32 glyphOffset = READ_LE_UINT32(data + offsetTableStart + i * 4);
			uint32 absOffset = 21 + glyphOffset;
			if (absOffset + 4 > dataSize)
				continue;

			byte glyphWidth = data[absOffset];
			byte glyphHeight = data[absOffset + 1];
			if (glyphWidth == 0 || glyphHeight == 0)
				continue;

			int col = i % cols;
			int row = i / cols;
			ImVec2 rectMin(imagePos.x + col * cellWidth * scale, imagePos.y + row * cellHeight * scale);
			ImVec2 rectMax(rectMin.x + glyphWidth * scale, rectMin.y + glyphHeight * scale);
			drawList->AddRect(rectMin, rectMax, outlineColor, 0.0f, 0, 1.0f);
		}
	}

	ImGui::EndChild();

	ImGui::EndChild();
}

void Charset::render(ImGuiID dockSpaceId, bool *open) {
	ImGui::SetNextWindowDockID(dockSpaceId, ImGuiCond_FirstUseEver);
	ImGui::Begin(ICON_CHARSET " Charset", open);

	renderCharsetList();
	ImGui::SameLine();
	renderCharsetProperties();
	ImGui::SameLine();

	CHR *chr = (_selectedCharset >= 0) ? findCharset(_selectedCharset) : nullptr;
	LFLF *lflf = (_selectedCharset >= 0) ? findLFLF(_selectedCharset) : nullptr;

	if (_selectedPaletteRoom >= 0)
		lflf = findLFLFByRoom(_selectedPaletteRoom);

	renderCharsetImage(chr, lflf);

	ImGui::End();
}

} // End of namespace Editor

} // End of namespace Scumm
