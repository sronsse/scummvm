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

#include "scumm/bomp.h"
#include "scumm/gfx.h"

#include "scumm/editor/editor.h"
#include "scumm/editor/style.h"

namespace Scumm {

namespace Editor {

static const byte BOMP_TRANSPARENT = 255;

static void drawBox(ImDrawList *drawList, const Box &box, ImVec2 origin, float scale, ImU32 outline, float thickness, ImU32 fill = 0) {
	ImVec2 points[4] = {
		ImVec2(origin.x + box.ulx * scale, origin.y + box.uly * scale),
		ImVec2(origin.x + box.urx * scale, origin.y + box.ury * scale),
		ImVec2(origin.x + box.lrx * scale, origin.y + box.lry * scale),
		ImVec2(origin.x + box.llx * scale, origin.y + box.lly * scale),
	};
	if (fill)
		drawList->AddConvexPolyFilled(points, 4, fill);
	drawList->AddPolyline(points, 4, outline, ImDrawFlags_Closed, thickness);
}

static void decodeObjectImage(byte *dst, int width, int height, const IMxx &imxx) {
	if (!imxx.bomp.data.empty()) {
		decompressBomp(dst, imxx.bomp.data.data(), width, height);
	} else if (!imxx.smap.data.empty()) {
		int numStrips = width / 8;
		const byte *smapData = imxx.smap.data.data();
		uint32 smapSize = imxx.smap.data.size();

		for (int strip = 0; strip < numStrips; strip++) {
			if ((uint32)(strip * 4 + 4) > smapSize)
				break;

			uint32 offset = READ_LE_UINT32(smapData + strip * 4);
			if (offset < 8 || (offset - 8) >= smapSize)
				continue;

			Gdi::decodeStrip(dst + strip * 8, width, smapData + offset - 8, height);
		}
	}
}

static bool hasBomp(const IMxx &imxx) {
	return !imxx.bomp.data.empty();
}

static const OBIM *findObjectImage(LFLF *lflf, uint16 objectId) {
	for (uint i = 0; i < lflf->room.obims.size(); ++i) {
		if (lflf->room.obims[i].imhd.id == objectId)
			return &lflf->room.obims[i];
	}
	return nullptr;
}

static void decodeBackground(Graphics::Surface &surface, const SMAP &smap, int width, int height) {
	int numStrips = width / 8;
	const byte *smapData = smap.data.data();
	uint32 smapSize = smap.data.size();

	for (int strip = 0; strip < numStrips; strip++) {
		if ((uint32)(strip * 4 + 4) > smapSize)
			break;

		uint32 offset = READ_LE_UINT32(smapData + strip * 4);
		if (offset < 8 || (offset - 8) >= smapSize)
			continue;

		Gdi::decodeStrip((byte *)surface.getBasePtr(strip * 8, 0), surface.pitch, smapData + offset - 8, height);
	}
}

static void buildPalette(byte *palette, const APAL &apal) {
	memset(palette, 0, 256 * 3);
	for (uint i = 0; i < apal.colors.size() && i < 256; ++i) {
		palette[i * 3 + 0] = apal.colors[i].r;
		palette[i * 3 + 1] = apal.colors[i].g;
		palette[i * 3 + 2] = apal.colors[i].b;
	}
}

static bool visibilityToggle(bool visible) {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, visible ? SOL_BLUE : SOL_BASE01);
	bool toggled = ImGui::SmallButton(visible ? ICON_VISIBLE : ICON_INVISIBLE);
	ImGui::PopStyleColor(4);
	ImGui::SameLine();
	return toggled;
}

Room::Room(RNAM &rnam, DROO &droo, LECF &lecf)
	: _rnam(rnam),
	  _droo(droo),
	  _lecf(lecf),
	  _selectedRoom(-1),
	  _selectedObject(-1),
	  _selectedImage(0),
	  _showObjects(true),
	  _selectedBox(-1),
	  _showBoxes(true),
	  _selectedMask(-1),
	  _showMasks(true),
	  _selectedCycle(-1),
	  _roomTexture(nullptr),
	  _tooltipTexture(nullptr),
	  _tooltipRoomId(-1) {
}

Room::~Room() {
	if (_roomTexture)
		g_system->freeImGuiTexture(_roomTexture);
	if (_tooltipTexture)
		g_system->freeImGuiTexture(_tooltipTexture);
}

LFLF *Room::findRoom(int roomId) {
	for (uint i = 0; i < _lecf.loff.locations.size(); ++i) {
		if (_lecf.loff.locations[i] == roomId)
			return &_lecf.lflfs[i];
	}
	return nullptr;
}

void *Room::createRoomTexture(LFLF *lflf) {
	const RMHD &rmhd = lflf->room.rmhd;
	const APAL &apal = lflf->room.pals.wrap.apals[0];
	const SMAP &smap = lflf->room.rmim.im00.smap;
	uint16 width = rmhd.width;
	uint16 height = rmhd.height;
	if (width == 0 || height == 0 || smap.data.empty() || apal.colors.empty())
		return nullptr;

	// Decode background strips
	Graphics::Surface surface;
	surface.create(width, height, Graphics::PixelFormat::createFormatCLUT8());
	memset(surface.getPixels(), 0, surface.h * surface.pitch);
	decodeBackground(surface, smap, width, height);

	// Composite object images
	if (_showObjects) {
		byte trnsIndex = lflf->room.trns.index;
		for (uint oi = 0; oi < lflf->room.obims.size(); ++oi) {
			const OBIM &obim = lflf->room.obims[oi];
			if (obim.imxxs.empty())
				continue;

			const CDHD *cdhd = nullptr;
			int objIndex = -1;
			for (uint oj = 0; oj < lflf->room.obcds.size(); oj++) {
				if (lflf->room.obcds[oj].cdhd.id == obim.imhd.id) {
					if (oj < _objectVisibility.size() && !_objectVisibility[oj])
						break;
					// Skip selected object - it will be drawn on top of overlays
					if ((int)oj == _selectedObject)
						break;
					cdhd = &lflf->room.obcds[oj].cdhd;
					objIndex = (int)oj;
					break;
				}
			}
			if (!cdhd)
				continue;

			int imageIndex = (objIndex == _selectedObject && _selectedImage < (int)obim.imxxs.size()) ? _selectedImage : 0;
			const IMxx &imxx = obim.imxxs[imageIndex];
			if (imxx.bomp.data.empty() && imxx.smap.data.empty())
				continue;

			int objX = cdhd->x;
			int objY = cdhd->y;
			int objWidth = obim.imhd.width;
			int objHeight = obim.imhd.height;

			if (objWidth <= 0 || objHeight <= 0)
				continue;

			byte objTrns = hasBomp(imxx) ? BOMP_TRANSPARENT : trnsIndex;

			byte *pixelBuf = new byte[objWidth * objHeight];
			memset(pixelBuf, objTrns, objWidth * objHeight);
			decodeObjectImage(pixelBuf, objWidth, objHeight, imxx);

			for (int sy = 0; sy < objHeight; sy++) {
				int destY = objY + sy;
				if (destY < 0 || destY >= (int)height)
					continue;
				for (int sx = 0; sx < objWidth; sx++) {
					int destX = objX + sx;
					if (destX < 0 || destX >= (int)width)
						continue;
					byte pixel = pixelBuf[sy * objWidth + sx];
					if (pixel != objTrns) {
						*((byte *)surface.getBasePtr(destX, destY)) = pixel;
					}
				}
			}

			delete[] pixelBuf;
		}
	}

	// Build palette
	byte palette[256 * 3];
	buildPalette(palette, apal);

	// Apply color cycling
	if (_selectedCycle >= 0 && _selectedCycle < (int)lflf->room.cycl.cycles.size()) {
		const Cycle &cycle = lflf->room.cycl.cycles[_selectedCycle];
		if (cycle.freq > 0 && cycle.start < cycle.end) {
			int numColors = cycle.end - cycle.start + 1;
			int delay = 16384 / cycle.freq;
			int jiffies = (int)(ImGui::GetTime() * 60.0);
			int offset = (jiffies / delay) % numColors;
			if (cycle.flags & 2)
				offset = numColors - 1 - offset;  // Backwards

			// Create cycled palette entries
			byte cycledColors[256 * 3];
			for (int i = 0; i < numColors; ++i) {
				int srcIdx = cycle.start + i;
				int dstIdx = cycle.start + ((i + offset) % numColors);
				cycledColors[dstIdx * 3 + 0] = palette[srcIdx * 3 + 0];
				cycledColors[dstIdx * 3 + 1] = palette[srcIdx * 3 + 1];
				cycledColors[dstIdx * 3 + 2] = palette[srcIdx * 3 + 2];
			}
			// Copy cycled colors back
			for (int i = cycle.start; i <= cycle.end; ++i) {
				palette[i * 3 + 0] = cycledColors[i * 3 + 0];
				palette[i * 3 + 1] = cycledColors[i * 3 + 1];
				palette[i * 3 + 2] = cycledColors[i * 3 + 2];
			}
		}
	}

	void *texture = g_system->getImGuiTexture(surface, palette, 256);
	surface.free();
	return texture;
}

void Room::drawObjectImage(ImDrawList *drawList, ImVec2 pos, int scale, LFLF *lflf, uint16 objectId, int imageIndex, bool transparent) {
	const OBIM *obim = findObjectImage(lflf, objectId);
	if (!obim || obim->imxxs.empty())
		return;
	if (imageIndex < 0 || imageIndex >= (int)obim->imxxs.size())
		imageIndex = 0;

	const IMxx &imxx = obim->imxxs[imageIndex];
	if (imxx.bomp.data.empty() && imxx.smap.data.empty())
		return;

	const APAL &apal = lflf->room.pals.wrap.apals[0];
	if (apal.colors.empty())
		return;

	int objWidth = obim->imhd.width;
	int objHeight = obim->imhd.height;
	if (objWidth <= 0 || objHeight <= 0)
		return;

	// Decode and draw pixels
	byte objTrns = hasBomp(imxx) ? BOMP_TRANSPARENT : lflf->room.trns.index;
	byte *pixelBuf = new byte[objWidth * objHeight];
	memset(pixelBuf, objTrns, objWidth * objHeight);
	decodeObjectImage(pixelBuf, objWidth, objHeight, imxx);

	for (int y = 0; y < objHeight; ++y) {
		for (int x = 0; x < objWidth; ++x) {
			byte pixel = pixelBuf[y * objWidth + x];
			if (transparent && pixel == objTrns)
				continue;

			ImU32 pixelColor = IM_COL32(128, 128, 128, 255);
			if (pixel < apal.colors.size()) {
				const Color &c = apal.colors[pixel];
				pixelColor = IM_COL32(c.r, c.g, c.b, 255);
			}

			ImVec2 p1(pos.x + x * scale, pos.y + y * scale);
			ImVec2 p2(p1.x + scale, p1.y + scale);
			drawList->AddRectFilled(p1, p2, pixelColor);
		}
	}

	delete[] pixelBuf;
}

void Room::renderObjectTooltip(LFLF *lflf, uint16 objectId, int imageIndex) {
	const OBIM *obim = findObjectImage(lflf, objectId);
	if (!obim || obim->imxxs.empty())
		return;
	if (imageIndex < 0 || imageIndex >= (int)obim->imxxs.size())
		imageIndex = 0;

	const IMxx &imxx = obim->imxxs[imageIndex];
	if (imxx.smap.data.empty() && imxx.bomp.data.empty())
		return;

	int objWidth = obim->imhd.width;
	int objHeight = obim->imhd.height;
	if (objWidth == 0 || objHeight == 0)
		return;

	ImGui::BeginTooltip();
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	drawObjectImage(drawList, canvasPos, 1, lflf, objectId, imageIndex, true);
	ImGui::Dummy(ImVec2((float)objWidth, (float)objHeight));
	ImGui::EndTooltip();
}

void Room::renderObjectImagesTab(LFLF *lflf, uint16 objectId) {
	const OBIM *obim = findObjectImage(lflf, objectId);
	if (!obim) {
		ImGui::TextColored(SOL_BASE01, "No image data.");
		return;
	}

	const IMHD &imhd = obim->imhd;
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("IMHDProps", 2, flags)) {
		ImGui::TableSetupColumn("Property");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "ID");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", imhd.id);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Images");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", imhd.numImages);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Width");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", imhd.width);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Height");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", imhd.height);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Flags");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "0x%02X", imhd.flags);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Hotspots");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", (int)imhd.hotspots.size());

		ImGui::EndTable();
	}

	ImGui::Spacing();

	if (obim->imxxs.empty()) {
		ImGui::TextColored(SOL_BASE01, "No images.");
		return;
	}

	// Image list
	ImGui::BeginChild("ImageList", ImVec2(100, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Images");

	for (uint i = 0; i < obim->imxxs.size(); ++i) {
		char label[32];
		Common::sprintf_s(label, "Image %d", i + 1);
		if (ImGui::Selectable(label, _selectedImage == (int)i))
			_selectedImage = (int)i;
		if (ImGui::IsItemHovered())
			renderObjectTooltip(lflf, objectId, (int)i);
	}
	ImGui::EndChild();
	ImGui::SameLine();

	// Image preview
	ImGui::BeginChild("ImagePreview", ImVec2(0, 0), ImGuiChildFlags_Borders);
	if (_selectedImage >= 0 && _selectedImage < (int)obim->imxxs.size()) {
		renderObjectImagePreview(lflf, objectId, _selectedImage);
	} else {
		ImGui::TextColored(SOL_BASE01, "Select an image.");
	}
	ImGui::EndChild();
}

void Room::renderObjectImagePreview(LFLF *lflf, uint16 objectId, int imageIndex) {
	const OBIM *obim = findObjectImage(lflf, objectId);
	if (!obim || imageIndex < 0 || imageIndex >= (int)obim->imxxs.size())
		return;

	const IMxx &imxx = obim->imxxs[imageIndex];
	if (imxx.smap.data.empty() && imxx.bomp.data.empty()) {
		ImGui::TextColored(SOL_BASE01, "No image data.");
		return;
	}

	int objWidth = obim->imhd.width;
	int objHeight = obim->imhd.height;
	if (objWidth == 0 || objHeight == 0)
		return;

	// Hotspot info
	const Hotspot *hotspot = (imageIndex < (int)obim->imhd.hotspots.size()) ? &obim->imhd.hotspots[imageIndex] : nullptr;
	ImGui::SeparatorText("Image Hotspot");

	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	if (ImGui::BeginTable("HotspotProps", 2, flags)) {
		ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 60);
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "X");
		ImGui::TableNextColumn();
		if (hotspot)
			ImGui::TextColored(SOL_BASE0, "%d", hotspot->x);
		else
			ImGui::TextColored(SOL_BASE01, "N/A");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Y");
		ImGui::TableNextColumn();
		if (hotspot)
			ImGui::TextColored(SOL_BASE0, "%d", hotspot->y);
		else
			ImGui::TextColored(SOL_BASE01, "N/A");

		ImGui::EndTable();
	}

	ImGui::SeparatorText("Image Preview");

	// Controls
	static int imgPreviewScale = 1;
	static bool imgPreviewTransparent = true;
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("Scale##imgpreview", &imgPreviewScale, 1, 8, "%dx");
	ImGui::SameLine();
	ImGui::Checkbox("Transparent##imgpreview", &imgPreviewTransparent);

	ImGui::BeginChild("ImgPreview", ImVec2(0, 0), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();

	int scale = imgPreviewScale;
	drawObjectImage(drawList, canvasPos, scale, lflf, objectId, imageIndex, imgPreviewTransparent);

	ImGui::Dummy(ImVec2((float)(objWidth * scale), (float)(objHeight * scale)));
	ImGui::EndChild();
}

void *Room::createTooltipTexture(LFLF *lflf) {
	const RMHD &rmhd = lflf->room.rmhd;
	const APAL &apal = lflf->room.pals.wrap.apals[0];
	const SMAP &smap = lflf->room.rmim.im00.smap;
	uint16 width = rmhd.width;
	uint16 height = rmhd.height;
	if (width == 0 || height == 0 || smap.data.empty() || apal.colors.empty())
		return nullptr;

	// Decode background strips
	Graphics::Surface surface;
	surface.create(width, height, Graphics::PixelFormat::createFormatCLUT8());
	memset(surface.getPixels(), 0, surface.h * surface.pitch);
	decodeBackground(surface, smap, width, height);

	// Build palette
	byte palette[256 * 3];
	buildPalette(palette, apal);

	void *texture = g_system->getImGuiTexture(surface, palette, 256);
	surface.free();
	return texture;
}

void Room::renderRoomTooltip(int roomId) {
	LFLF *lflf = findRoom(roomId);
	if (!lflf)
		return;

	const RMHD &rmhd = lflf->room.rmhd;
	if (rmhd.width == 0 || rmhd.height == 0)
		return;

	// Update texture if room changed
	if (_tooltipRoomId != roomId) {
		if (_tooltipTexture)
			g_system->freeImGuiTexture(_tooltipTexture);
		_tooltipTexture = createTooltipTexture(lflf);
		_tooltipRoomId = roomId;
	}
	if (!_tooltipTexture)
		return;

	ImGui::BeginTooltip();
	ImGui::Image((ImTextureID)(intptr_t)_tooltipTexture, ImVec2((float)rmhd.width, (float)rmhd.height));
	ImGui::EndTooltip();
}

void Room::renderRoomList() {
	ImGui::BeginChild("RoomList", ImVec2(200, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Rooms");

	for (uint i = 0; i < _droo.locations.size(); ++i) {
		if (_droo.locations[i] == 0)
			continue;

		int roomId = (int)i;
		const char *roomName = "";
		for (uint j = 0; j < _rnam.locations.size(); ++j) {
			if (_rnam.locations[j] == roomId) {
				roomName = _rnam.names[j].c_str();
				break;
			}
		}

		char label[64];
		if (roomName[0] != '\0')
			Common::sprintf_s(label, "Room %d: %s", roomId, roomName);
		else
			Common::sprintf_s(label, "Room %d", roomId);
		if (ImGui::Selectable(label, _selectedRoom == roomId)) {
			if (_selectedRoom != roomId) {
				_selectedObject = -1;
				_selectedImage = 0;
				_selectedBox = -1;
				_selectedMask = -1;
				_selectedCycle = -1;
				_objectVisibility.clear();
				_maskVisibility.clear();
			}
			_selectedRoom = roomId;
		}
		if (ImGui::IsItemHovered()) {
			renderRoomTooltip(roomId);
		}
	}

	ImGui::EndChild();
}

void Room::renderRoomProperties() {
	ImGui::BeginChild("RoomProperties", ImVec2(400, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

	if (_selectedRoom < 0) {
		ImGui::TextColored(SOL_BASE01, "Select a room.");
		ImGui::EndChild();
		return;
	}

	LFLF *lflf = findRoom(_selectedRoom);
	if (!lflf) {
		ImGui::TextColored(SOL_BASE01, "Room data not found.");
		ImGui::EndChild();
		return;
	}

	// Get room name index
	int rnamIndex = -1;
	for (uint i = 0; i < _rnam.locations.size(); ++i) {
		if (_rnam.locations[i] == _selectedRoom) {
			rnamIndex = (int)i;
			break;
		}
	}

	// Room header
	char headerBuf[64];
	Common::sprintf_s(headerBuf, "Room %d", _selectedRoom);
	ImGui::SeparatorText(headerBuf);
	if (rnamIndex >= 0)
		ImGui::TextColored(SOL_BASE0, "%s", _rnam.names[rnamIndex].c_str());
	ImGui::Separator();

	const RMHD &rmhd = lflf->room.rmhd;
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("RoomHeader", 2, flags)) {
		ImGui::TableSetupColumn("Property");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Width");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", rmhd.width);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Height");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", rmhd.height);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Objects");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", rmhd.numObjects);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Masks");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", lflf->room.rmim.rmih.numZPlanes);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Transparent");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", lflf->room.trns.index);

		ImGui::EndTable();
	}

	if (ImGui::BeginTabBar("RoomPropsTabs")) {
		if (ImGui::BeginTabItem("Palette")) {
			renderPalette(lflf);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Cycles")) {
			renderCyclesList(lflf);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Masks")) {
			renderMasksList(lflf);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Boxes")) {
			renderBoxesList(lflf);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Objects")) {
			renderObjectsList(lflf);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::EndChild();
}

void Room::renderPalette(LFLF *lflf) {
	if (lflf->room.pals.wrap.apals.empty()) {
		ImGui::TextColored(SOL_BASE01, "No palette data.");
		return;
	}

	Common::Array<Color> &colors = lflf->room.pals.wrap.apals[0].colors;

	ImGui::BeginChild("PaletteGrid", ImVec2(0, 0), ImGuiChildFlags_Borders);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	for (uint i = 0; i < colors.size(); ++i) {
		ImGui::PushID(i);

		float col[3] = {
			colors[i].r / 255.0f,
			colors[i].g / 255.0f,
			colors[i].b / 255.0f
		};
		ImGui::ColorEdit3("##color", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker);

		if ((i % 16) != 15)
			ImGui::SameLine();

		ImGui::PopID();
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void Room::renderCyclesList(LFLF *lflf) {
	const Common::Array<Cycle> &cycles = lflf->room.cycl.cycles;
	if (cycles.empty()) {
		ImGui::TextColored(SOL_BASE01, "No color cycles.");
		return;
	}

	ImGui::BeginChild("CycleList", ImVec2(100, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Cycles");

	for (uint i = 0; i < cycles.size(); ++i) {
		char label[32];
		Common::sprintf_s(label, "Cycle %d", cycles[i].id);
		if (ImGui::Selectable(label, _selectedCycle == (int)i)) {
			_selectedCycle = (int)i;
			_selectedObject = -1;
			_selectedBox = -1;
			_selectedMask = -1;
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("CycleProperties", ImVec2(0, 0), ImGuiChildFlags_Borders);
	if (_selectedCycle >= 0 && _selectedCycle < (int)cycles.size()) {
		const Cycle &cycle = cycles[_selectedCycle];

		char headerBuf[64];
		Common::sprintf_s(headerBuf, "Cycle %d Properties", cycle.id);
		ImGui::SeparatorText(headerBuf);

		const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
		if (ImGui::BeginTable("CycleProps", 2, flags)) {
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 80);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BLUE, "Frequency");
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BASE0, "%d", cycle.freq);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BLUE, "Delay");
			ImGui::TableNextColumn();
			if (cycle.freq > 0)
				ImGui::TextColored(SOL_BASE0, "%d", 16384 / cycle.freq);
			else
				ImGui::TextColored(SOL_BASE01, "N/A");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BLUE, "Flags");
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BASE0, "0x%04X%s", cycle.flags, (cycle.flags & 2) ? " (backwards)" : "");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BLUE, "Start");
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BASE0, "%d", cycle.start);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BLUE, "End");
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BASE0, "%d", cycle.end);

			ImGui::EndTable();
		}

		Common::sprintf_s(headerBuf, "Cycle %d Preview", cycle.id);
		ImGui::SeparatorText(headerBuf);

		if (lflf->room.pals.wrap.apals.empty() || cycle.freq == 0) {
			ImGui::TextColored(SOL_BASE01, "No palette or invalid frequency.");
		} else {
			const Common::Array<Color> &colors = lflf->room.pals.wrap.apals[0].colors;
			int numColors = cycle.end - cycle.start + 1;
			if (numColors > 0 && cycle.start < colors.size() && cycle.end < colors.size()) {
				int delay = 16384 / cycle.freq;
				int jiffies = (int)(ImGui::GetTime() * 60.0);
				int offset = (jiffies / delay) % numColors;
				if (cycle.flags & 2)
					offset = numColors - 1 - offset;  // Backwards

				// Display cycling color
				int colorIdx = cycle.start + offset;
				if (colorIdx < (int)colors.size()) {
					const Color &c = colors[colorIdx];
					float col[3] = { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f };
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
					ImGui::ColorEdit3("##cyclepreview", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);
					ImGui::PopStyleVar();
					ImGui::SameLine();
					ImGui::TextColored(SOL_BASE0, "Color %d", colorIdx);
				}
			}
		}
	} else {
		ImGui::TextColored(SOL_BASE01, "Select a cycle.");
	}
	ImGui::EndChild();
}

void Room::renderMasksList(LFLF *lflf) {
	const Common::Array<ZPxx> &masks = lflf->room.rmim.im00.zpxxs;
	if (masks.empty()) {
		ImGui::TextColored(SOL_BASE01, "No masks.");
		return;
	}

	// Initialize visibility
	if (_maskVisibility.size() != masks.size()) {
		_maskVisibility.resize(masks.size());
		for (uint i = 0; i < masks.size(); ++i)
			_maskVisibility[i] = true;
	}

	// Mask list
	ImGui::BeginChild("MaskList", ImVec2(100, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Masks");

	for (uint i = 0; i < masks.size(); ++i) {
		ImGui::PushID(i);

		if (visibilityToggle(_maskVisibility[i]))
			_maskVisibility[i] = !_maskVisibility[i];

		char label[32];
		Common::sprintf_s(label, "Mask %d", i + 1);
		if (ImGui::Selectable(label, _selectedMask == (int)i)) {
			_selectedMask = (int)i;
			_selectedObject = -1;
			_selectedBox = -1;
			_selectedCycle = -1;
		}
		if (ImGui::IsItemHovered())
			renderMaskTooltip(lflf, (int)i);

		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("MaskPreview", ImVec2(0, 0), ImGuiChildFlags_Borders);
	if (_selectedMask >= 0 && _selectedMask < (int)masks.size()) {
		ImGui::SeparatorText("Mask Preview");

		const ZPxx &mask = masks[_selectedMask];
		if (mask.data.empty()) {
			ImGui::TextColored(SOL_BASE01, "No mask data.");
		} else {
			uint16 width = lflf->room.rmhd.width;
			uint16 height = lflf->room.rmhd.height;
			static int maskPreviewScale = 1;
			static bool maskPreviewTransparent = true;
			ImGui::SetNextItemWidth(150);
			ImGui::SliderInt("Scale##maskpreview", &maskPreviewScale, 1, 8, "%dx");
			ImGui::SameLine();
			ImGui::Checkbox("Transparent##maskpreview", &maskPreviewTransparent);

			ImGui::BeginChild("MaskPreviewScroll", ImVec2(0, 0), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
			ImDrawList *drawList = ImGui::GetWindowDrawList();
			ImVec2 canvasPos = ImGui::GetCursorScreenPos();
			int scale = maskPreviewScale;

			// Draw background
			if (!maskPreviewTransparent) {
				ImVec2 bgMax(canvasPos.x + width * scale, canvasPos.y + height * scale);
				drawList->AddRectFilled(canvasPos, bgMax, IM_COL32(0, 0, 0, 255));
			}

			drawMask(drawList, canvasPos, (float)scale, lflf, _selectedMask, IM_COL32(255, 255, 255, 255));

			ImGui::Dummy(ImVec2((float)(width * scale), (float)(height * scale)));
			ImGui::EndChild();
		}
	} else {
		ImGui::TextColored(SOL_BASE01, "Select a mask.");
	}
	ImGui::EndChild();
}

void Room::drawMask(ImDrawList *drawList, ImVec2 pos, float scale, LFLF *lflf, int maskIndex, ImU32 color) {
	const Common::Array<ZPxx> &masks = lflf->room.rmim.im00.zpxxs;
	if (maskIndex < 0 || maskIndex >= (int)masks.size())
		return;

	const ZPxx &mask = masks[maskIndex];
	if (mask.data.empty())
		return;

	uint16 width = lflf->room.rmhd.width;
	uint16 height = lflf->room.rmhd.height;
	int numStrips = width / 8;
	const byte *maskData = mask.data.data();
	uint32 maskSize = mask.data.size();

	Common::Array<byte> decoded(numStrips * height, 0);

	for (int strip = 0; strip < numStrips; ++strip) {
		if ((uint32)(strip * 2 + 2) > maskSize)
			break;
		uint16 offs = READ_LE_UINT16(maskData + strip * 2);
		if (offs < 8 || (uint32)(offs - 8) >= maskSize)
			continue;

		Gdi::decodeMaskStrip(&decoded[strip * height], 1, maskData + offs - 8, height);
	}

	for (int y = 0; y < height; ++y) {
		for (int strip = 0; strip < numStrips; ++strip) {
			byte b = decoded[strip * height + y];
			for (int bit = 0; bit < 8; ++bit) {
				if (b & (0x80 >> bit)) {
					int px = strip * 8 + bit;
					ImVec2 p1(pos.x + px * scale, pos.y + y * scale);
					ImVec2 p2(p1.x + scale, p1.y + scale);
					drawList->AddRectFilled(p1, p2, color);
				}
			}
		}
	}
}

void Room::renderMaskTooltip(LFLF *lflf, int maskIndex) {
	const Common::Array<ZPxx> &masks = lflf->room.rmim.im00.zpxxs;
	if (maskIndex < 0 || maskIndex >= (int)masks.size())
		return;

	if (masks[maskIndex].data.empty())
		return;

	uint16 width = lflf->room.rmhd.width;
	uint16 height = lflf->room.rmhd.height;

	if (width == 0 || height == 0)
		return;

	ImGui::BeginTooltip();
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();

	drawMask(drawList, canvasPos, 1.0f, lflf, maskIndex, IM_COL32(255, 255, 255, 255));

	ImGui::Dummy(ImVec2((float)width, (float)height));
	ImGui::EndTooltip();
}

void Room::renderBoxesList(LFLF *lflf) {
	Common::Array<Box> &boxes = lflf->room.boxd.boxes;
	if (boxes.empty())
		return;

	// Box list
	ImGui::BeginChild("BoxList", ImVec2(150, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Boxes");

	for (uint i = 1; i < boxes.size(); ++i) {
		char label[32];
		Common::sprintf_s(label, "Box %d", i);
		if (ImGui::Selectable(label, _selectedBox == (int)i)) {
			_selectedBox = (int)i;
			_selectedObject = -1;
			_selectedMask = -1;
			_selectedCycle = -1;
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("BoxProperties", ImVec2(0, 0), ImGuiChildFlags_Borders);
	if (_selectedBox >= 0 && _selectedBox < (int)boxes.size()) {
		renderBoxProperties(lflf, _selectedBox, boxes[_selectedBox]);
	} else {
		ImGui::TextColored(SOL_BASE01, "Select a box.");
	}
	ImGui::EndChild();
}

void Room::renderBoxProperties(LFLF *lflf, int boxIndex, Box &box) {
	char headerBuf[64];
	Common::sprintf_s(headerBuf, "Box %d Data", boxIndex);
	ImGui::SeparatorText(headerBuf);
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("BoxProps", 2, flags)) {
		ImGui::TableSetupColumn("Property");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		auto row = [](const char *label, const char *fmt, int value) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BLUE, "%s", label);
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BASE0, fmt, value);
		};

		row("Upper Left X", "%d", box.ulx);
		row("Upper Left Y", "%d", box.uly);
		row("Upper Right X", "%d", box.urx);
		row("Upper Right Y", "%d", box.ury);
		row("Lower Right X", "%d", box.lrx);
		row("Lower Right Y", "%d", box.lry);
		row("Lower Left X", "%d", box.llx);
		row("Lower Left Y", "%d", box.lly);
		row("Mask", "%d", box.mask);
		row("Flags", "%02X", box.flags);
		row("Scale", "%d", box.scale);

		ImGui::EndTable();
	}

	Common::sprintf_s(headerBuf, "Box %d Matrix", boxIndex);
	ImGui::SeparatorText(headerBuf);

	const BOXM &boxm = lflf->room.boxm;
	if (boxIndex < (int)boxm.matrix.size()) {
		const Common::Array<Entry> &entries = boxm.matrix[boxIndex];
		if (entries.empty()) {
			ImGui::TextColored(SOL_BASE01, "No matrix entries.");
		} else {
			const ImVec4 colYellow(1.0f, 1.0f, 0.0f, 1.0f);
			const ImVec4 colFuschia(0.93f, 0.3f, 0.93f, 1.0f);
			const ImVec4 colBlue(0.4f, 0.45f, 1.0f, 1.0f);

			if (ImGui::BeginTable("BoxMatrix", 2, flags)) {
				ImGui::TableSetupColumn("Dest");
				ImGui::TableSetupColumn("Next");
				ImGui::TableHeadersRow();

				for (uint i = 0; i < entries.size(); ++i) {
					const Entry &entry = entries[i];
					// Expand the range into individual rows
					for (int dest = entry.from; dest <= entry.to; ++dest) {
						// Skip invalid box 0
						if (dest == 0)
							continue;

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						if (dest == boxIndex) {
							ImGui::TextColored(colYellow, "Box %d", dest);
						} else if (entry.next == dest) {
							ImGui::TextColored(colFuschia, "Box %d", dest);
						} else if (entry.next != 0) {
							ImGui::TextColored(colBlue, "Box %d", dest);
						} else {
							ImGui::TextColored(SOL_BASE01, "Box %d", dest);
						}
						ImGui::TableNextColumn();
						if (dest == boxIndex) {
							ImGui::TextColored(colYellow, "Current");
						} else if (entry.next == 0) {
							ImGui::TextColored(SOL_BASE01, "None");
						} else if (entry.next == dest) {
							ImGui::TextColored(colFuschia, "Direct");
						} else {
							ImGui::TextColored(colBlue, "Box %d", entry.next);
						}
					}
				}

				ImGui::EndTable();
			}
		}
	} else {
		ImGui::TextColored(SOL_BASE01, "No matrix data.");
	}
}

void Room::renderObjectsList(LFLF *lflf) {
	Common::Array<OBCD> &obcds = lflf->room.obcds;
	if (obcds.empty())
		return;

	// Initialize visibility
	if (_objectVisibility.size() != obcds.size()) {
		_objectVisibility.resize(obcds.size());
		for (uint i = 0; i < obcds.size(); ++i)
			_objectVisibility[i] = true;
	}

	// Object list
	ImGui::BeginChild("ObjectList", ImVec2(150, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Objects");

	for (uint i = 0; i < obcds.size(); ++i) {
		ImGui::PushID(i);

		if (visibilityToggle(_objectVisibility[i]))
			_objectVisibility[i] = !_objectVisibility[i];

		char label[128];
		if (!obcds[i].obna.name.empty())
			Common::sprintf_s(label, "%s##obj%d", obcds[i].obna.name.c_str(), obcds[i].cdhd.id);
		else
			Common::sprintf_s(label, "Object %d", obcds[i].cdhd.id);
		if (ImGui::Selectable(label, _selectedObject == (int)i)) {
			if (_selectedObject != (int)i)
				_selectedImage = 0;
			_selectedObject = (int)i;
			_selectedBox = -1;
			_selectedMask = -1;
			_selectedCycle = -1;
		}
		if (ImGui::IsItemHovered()) {
			renderObjectTooltip(lflf, obcds[i].cdhd.id);
		}

		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("ObjectProperties", ImVec2(0, 0), ImGuiChildFlags_Borders);
	if (_selectedObject >= 0 && _selectedObject < (int)obcds.size()) {
		renderObjectProperties(lflf, obcds[_selectedObject]);
	} else {
		ImGui::TextColored(SOL_BASE01, "Select an object.");
	}
	ImGui::EndChild();
}

void Room::renderObjectProperties(LFLF *lflf, OBCD &obcd) {
	CDHD &cdhd = obcd.cdhd;

	char headerBuf[64];
	Common::sprintf_s(headerBuf, "Object %d Properties", cdhd.id);
	ImGui::SeparatorText(headerBuf);

	if (ImGui::BeginTabBar("ObjectPropsTabs")) {
		if (ImGui::BeginTabItem("Images")) {
			renderObjectImagesTab(lflf, cdhd.id);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Code")) {
			renderObjectCodeProperties(lflf, obcd);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Room::renderObjectCodeProperties(LFLF *lflf, OBCD &obcd) {
	CDHD &cdhd = obcd.cdhd;
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("ObjectProps", 2, flags)) {
		ImGui::TableSetupColumn("Property");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "ID");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", cdhd.id);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Name");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%s", obcd.obna.name.c_str());

		auto row = [](const char *label, const char *fmt, int value) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BLUE, "%s", label);
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BASE0, fmt, value);
		};

		row("X", "%d", cdhd.x);
		row("Y", "%d", cdhd.y);
		row("Width", "%d", cdhd.width);
		row("Height", "%d", cdhd.height);
		row("Flags", "%02X", cdhd.flags);
		row("Parent", "%d", cdhd.parent);
		row("Actor Dir", "%d", cdhd.actorDir);

		ImGui::EndTable();
	}
}

void Room::renderRoomImage() {
	ImGui::BeginChild("RoomImage", ImVec2(0, 0), ImGuiChildFlags_Borders);

	ImGui::SeparatorText("Room Preview");

	if (_selectedRoom < 0) {
		ImGui::EndChild();
		return;
	}

	LFLF *lflf = findRoom(_selectedRoom);
	if (!lflf) {
		ImGui::EndChild();
		return;
	}

	float roomW = (float)lflf->room.rmhd.width;
	float roomH = (float)lflf->room.rmhd.height;

	// Controls
	static int roomScale = 2;
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("Scale", &roomScale, 1, 8, "%dx");
	ImGui::SameLine();
	ImGui::Checkbox("Masks", &_showMasks);
	ImGui::SameLine();
	ImGui::Checkbox("Boxes", &_showBoxes);
	ImGui::SameLine();
	ImGui::Checkbox("Objects", &_showObjects);

	// Create room texture
	if (_roomTexture)
		g_system->freeImGuiTexture(_roomTexture);
	_roomTexture = createRoomTexture(lflf);
	if (!_roomTexture) {
		ImGui::EndChild();
		return;
	}

	// Color scheme
	struct ColorScheme { ImVec4 objOutline, objFill, boxOutline, boxFill, boxConnOutline, boxConnFill, boxSelOutline, boxSelFill; ImU32 mask, maskSel; };
	const ColorScheme cs = {
		ImVec4(0.3f, 0.6f, 0.3f, 1.0f), ImVec4(1.0f, 1.0f, 0.0f, 0.75f),
		ImVec4(0.4f, 0.45f, 1.0f, 1.0f), ImVec4(0.4f, 0.45f, 1.0f, 0.75f),
		ImVec4(0.93f, 0.3f, 0.93f, 1.0f), ImVec4(0.93f, 0.3f, 0.93f, 0.75f),
		ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ImVec4(1.0f, 1.0f, 0.0f, 0.75f),
		IM_COL32(255, 89, 89, 191), IM_COL32(255, 255, 0, 191)
	};

	ImGui::BeginChild("RoomImageScroll", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

	float scale = (float)roomScale;
	ImVec2 imageSize(roomW * scale, roomH * scale);
	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)(intptr_t)_roomTexture, imageSize);

	ImDrawList *drawList = ImGui::GetWindowDrawList();

	// Clip overlays to room bounds
	ImVec2 clipMin = imagePos;
	ImVec2 clipMax(imagePos.x + imageSize.x, imagePos.y + imageSize.y);
	drawList->PushClipRect(clipMin, clipMax, true);

	// Draw unselected masks
	auto drawUnselectedMasks = [&]() {
		if (!_showMasks || lflf->room.rmim.im00.zpxxs.empty())
			return;
		ImU32 maskColor = cs.mask;
		for (uint i = 0; i < lflf->room.rmim.im00.zpxxs.size(); ++i) {
			if (_selectedMask == (int)i)
				continue;
			if (i < _maskVisibility.size() && !_maskVisibility[i])
				continue;
			drawMask(drawList, imagePos, scale, lflf, (int)i, maskColor);
		}
	};

	// Draw selected mask
	auto drawSelectedMask = [&]() {
		if (!_showMasks || lflf->room.rmim.im00.zpxxs.empty())
			return;
		if (_selectedMask >= 0 && _selectedMask < (int)lflf->room.rmim.im00.zpxxs.size()) {
			if (_maskVisibility.empty() || _maskVisibility[_selectedMask])
				drawMask(drawList, imagePos, scale, lflf, _selectedMask, cs.maskSel);
		}
	};

	// Draw unselected boxes
	auto drawUnselectedBoxes = [&]() {
		if (!_showBoxes || lflf->room.boxd.boxes.size() <= 1)
			return;
		ImU32 boxOutline = ImGui::GetColorU32(cs.boxOutline);
		ImU32 boxFill = ImGui::GetColorU32(cs.boxFill);
		ImU32 connectedOutline = ImGui::GetColorU32(cs.boxConnOutline);
		ImU32 connectedFill = ImGui::GetColorU32(cs.boxConnFill);

		Common::Array<bool> isDirectlyConnected(lflf->room.boxd.boxes.size(), false);
		if (_selectedBox > 0 && _selectedBox < (int)lflf->room.boxm.matrix.size()) {
			const Common::Array<Entry> &entries = lflf->room.boxm.matrix[_selectedBox];
			for (uint i = 0; i < entries.size(); ++i) {
				const Entry &entry = entries[i];
				for (int dest = entry.from; dest <= entry.to; ++dest) {
					if (dest > 0 && dest < (int)isDirectlyConnected.size() && entry.next == dest)
						isDirectlyConnected[dest] = true;
				}
			}
		}

		// Draw normal boxes first
		for (uint i = 1; i < lflf->room.boxd.boxes.size(); ++i) {
			if (_selectedBox == (int)i || isDirectlyConnected[i])
				continue;
			drawBox(drawList, lflf->room.boxd.boxes[i], imagePos, scale, boxOutline, 1.0f, boxFill);
		}
		for (uint i = 1; i < lflf->room.boxd.boxes.size(); ++i) {
			if (!isDirectlyConnected[i])
				continue;
			drawBox(drawList, lflf->room.boxd.boxes[i], imagePos, scale, connectedOutline, 1.5f, connectedFill);
		}
	};

	// Draw selected box
	auto drawSelectedBox = [&]() {
		if (!_showBoxes || lflf->room.boxd.boxes.size() <= 1)
			return;
		if (_selectedBox > 0 && _selectedBox < (int)lflf->room.boxd.boxes.size()) {
			ImU32 selectedOutline = ImGui::GetColorU32(cs.boxSelOutline);
			ImU32 selectedFill = ImGui::GetColorU32(cs.boxSelFill);
			drawBox(drawList, lflf->room.boxd.boxes[_selectedBox], imagePos, scale, selectedOutline, 2.0f, selectedFill);
		}
	};

	// Draw unselected objects
	auto drawUnselectedObjects = [&]() {
		if (!_showObjects || lflf->room.obcds.empty())
			return;
		ImU32 outlineColor = ImGui::GetColorU32(cs.objOutline);
		ImU32 normalFill = ImGui::GetColorU32(ImVec4(cs.objOutline.x, cs.objOutline.y, cs.objOutline.z, 0.75f));

		for (uint i = 0; i < lflf->room.obcds.size(); ++i) {
			if (_selectedObject == (int)i)
				continue;
			if (i < _objectVisibility.size() && !_objectVisibility[i])
				continue;
			const CDHD &cdhd = lflf->room.obcds[i].cdhd;
			ImVec2 rectMin(imagePos.x + cdhd.x * scale, imagePos.y + cdhd.y * scale);
			ImVec2 rectMax(rectMin.x + cdhd.width * scale, rectMin.y + cdhd.height * scale);
			drawList->AddRectFilled(rectMin, rectMax, normalFill);
			drawList->AddRect(rectMin, rectMax, outlineColor, 0.0f, 0, 1.0f);
		}
	};

	// Draw selected object
	auto drawSelectedObject = [&]() {
		if (_selectedObject >= 0 && _selectedObject < (int)lflf->room.obcds.size()) {
			if (_objectVisibility.empty() || _objectVisibility[_selectedObject]) {
				const CDHD &cdhd = lflf->room.obcds[_selectedObject].cdhd;
				ImVec2 objPos(imagePos.x + cdhd.x * scale, imagePos.y + cdhd.y * scale);
				drawObjectImage(drawList, objPos, (int)scale, lflf, cdhd.id, _selectedImage, true);
				ImVec2 rectMax(objPos.x + cdhd.width * scale, objPos.y + cdhd.height * scale);
				drawList->AddRectFilled(objPos, rectMax, ImGui::GetColorU32(cs.objFill));
				drawList->AddRect(objPos, rectMax, IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);
			}
		}
	};

	// Render overlays (selected layer on top)
	if (_selectedObject >= 0) {
		drawUnselectedMasks();
		drawUnselectedBoxes();
		drawUnselectedObjects();
		drawSelectedObject();
	} else if (_selectedBox >= 0) {
		drawUnselectedMasks();
		drawUnselectedObjects();
		drawUnselectedBoxes();
		drawSelectedBox();
	} else if (_selectedMask >= 0) {
		drawUnselectedBoxes();
		drawUnselectedObjects();
		drawUnselectedMasks();
		drawSelectedMask();
	} else {
		drawUnselectedMasks();
		drawUnselectedBoxes();
		drawUnselectedObjects();
	}

	drawList->PopClipRect();

	ImGui::EndChild();

	ImGui::EndChild();
}

void Room::render(ImGuiID dockSpaceId, bool *open) {
	ImGui::SetNextWindowDockID(dockSpaceId, ImGuiCond_FirstUseEver);
	ImGui::Begin(ICON_ROOM " Room", open);

	renderRoomList();
	ImGui::SameLine();
	renderRoomProperties();
	ImGui::SameLine();
	renderRoomImage();

	ImGui::End();
}

} // End of namespace Editor

} // End of namespace Scumm
