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
#include "common/hashmap.h"

#include "scumm/editor/costume.h"
#include "scumm/editor/style.h"

namespace Scumm {

namespace Editor {

static const char *animGroupNames[] = { "unknown", "init", "walk", "stand", "talk start", "talk stop" };
static const int numStandardGroups = 6;
static const char *directionNames[] = { "West", "East", "South", "North" };

Costume::Costume(DCOS &dcos, LECF &lecf)
	: _dcos(dcos),
	  _lecf(lecf),
	  _selectedCostume(-1),
	  _selectedPaletteRoom(-1),
	  _selectedAnimGroup(-1),
	  _selectedDirection(DIR_WEST),
	  _selectedLimb(-1),
	  _selectedCommand(-1),
	  _showBoundingBox(true),
	  _showAnimBounds(true),
	  _animCounter(0),
	  _animMaxCounter(0),
	  _animPlaying(false),
	  _animSpeed(5),
	  _animProgress(0),
	  _animFrameAccum(0.0f),
	  _lastAnimGroup(-1),
	  _lastDirection(-1),
	  _lastCostume(-1) {

	// Initialize animation state
	for (int i = 0; i < 16; i++) {
		_animCurPos[i] = 0xFFFF;
		_animStart[i] = 0;
		_animEnd[i] = 0;
		_animLooping[i] = false;
	}
}

COST *Costume::findCostume(int costumeId) {
	if (costumeId < 0 || costumeId >= (int)_dcos.locations.size())
		return nullptr;

	LFLF *lflf = findLFLFByRoom(_dcos.locations[costumeId]);
	if (!lflf)
		return nullptr;

	for (uint k = 0; k < lflf->costs.size(); ++k) {
		if (lflf->costs[k].id == costumeId)
			return &lflf->costs[k];
	}
	return nullptr;
}

LFLF *Costume::findLFLF(int costumeId) {
	if (costumeId < 0 || costumeId >= (int)_dcos.locations.size())
		return nullptr;
	return findLFLFByRoom(_dcos.locations[costumeId]);
}

LFLF *Costume::findLFLFByRoom(int roomNum) {
	if (roomNum <= 0)
		return nullptr;

	for (uint i = 0; i < _lecf.loff.locations.size(); ++i) {
		if (_lecf.loff.locations[i] == roomNum)
			return &_lecf.lflfs[i];
	}
	return nullptr;
}

const Common::Array<Color> *Costume::getRoomColors() {
	LFLF *lflf = (_selectedPaletteRoom < 0) ? findLFLF(_selectedCostume) : findLFLFByRoom(_selectedPaletteRoom);
	if (lflf && !lflf->room.pals.wrap.apals.empty())
		return &lflf->room.pals.wrap.apals[0].colors;
	return nullptr;
}

Common::Array<byte> Costume::getCostumePalette(const byte *data, uint32 size) {
	Common::Array<byte> palette;

	if (!data || size < 8)
		return palette;

	byte format = data[7];
	int numColors = (format & 0x01) ? 32 : 16;

	int paletteOffset = 8;
	if ((uint32)paletteOffset + numColors > size)
		return palette;

	palette.resize(numColors);
	for (int i = 0; i < numColors; ++i) {
		palette[i] = data[paletteOffset + i];
	}
	return palette;
}

PictureInfo Costume::getPictureInfo(const byte *data, uint32 size, byte format, int limbIdx, int picIdx) {
	PictureInfo info;

	int numColors = (format & 0x01) ? 32 : 16;
	int limbOffsetsPos = 8 + numColors + 2;

	if ((uint32)limbOffsetsPos + limbIdx * 2 + 2 > size)
		return info;

	uint16 limbOffset = READ_LE_UINT16(data + limbOffsetsPos + limbIdx * 2);
	if (limbOffset == 0 || (uint32)limbOffset + picIdx * 2 + 2 > size)
		return info;

	uint16 picOffset = READ_LE_UINT16(data + limbOffset + picIdx * 2);
	if (picOffset == 0 || (uint32)picOffset + 12 > size)
		return info;

	const byte *picData = data + picOffset;
	bool hasRedir = (format & 0x7E) == 0x60;

	// Read dimensions from picture header
	info.width = READ_LE_UINT16(picData);
	info.height = READ_LE_UINT16(picData + 2);
	info.relX = (int16)READ_LE_UINT16(picData + 4);
	info.relY = (int16)READ_LE_UINT16(picData + 6);

	// Handle format 0x60 with redirection
	if (hasRedir) {
		if ((uint32)picOffset + 14 > size)
			return info;

		byte redirLimb = picData[12];
		byte redirPict = picData[13];
		bool isRedir = (redirLimb != 0xFF || redirPict != 0xFF);

		if (isRedir) {
			info.redirected = true;
			info.redirLimb = redirLimb;
			info.redirPict = redirPict;

			// Look up target picture RLE data
			if ((uint32)limbOffsetsPos + redirLimb * 2 + 2 > size)
				return info;
			uint16 redirLimbOffset = READ_LE_UINT16(data + limbOffsetsPos + redirLimb * 2);
			if (redirLimbOffset == 0 || (uint32)redirLimbOffset + redirPict * 2 + 2 > size)
				return info;
			uint16 redirPicOffset = READ_LE_UINT16(data + redirLimbOffset + redirPict * 2);
			if (redirPicOffset == 0 || (uint32)redirPicOffset + 14 > size)
				return info;

			const byte *targetPic = data + redirPicOffset;
			info.rleData = targetPic + 14;
			info.rleSize = size - (uint32)(redirPicOffset + 14);
		} else {
			info.rleData = picData + 14;
			info.rleSize = size - (uint32)(picOffset + 14);
		}
	} else {
		info.rleData = picData + 12;
		info.rleSize = size - (uint32)(picOffset + 12);
	}

	info.valid = (info.width > 0 && info.height > 0 && info.width <= 256 && info.height <= 256);

	return info;
}

LimbDimensions Costume::getLimbMaxDimensions(const byte *data, uint32 size, byte format, int limbIdx, uint16 startOffset, uint16 endOffset, uint16 animCmdsOffset) {
	LimbDimensions dims;

	if ((uint32)animCmdsOffset >= size)
		return dims;

	const byte *animCmds = data + animCmdsOffset;

	// Scan commands for PIC commands to calculate bounding box
	for (uint16 cmdIdx = startOffset; cmdIdx <= endOffset; ++cmdIdx) {
		if ((uint32)animCmdsOffset + cmdIdx >= size)
			break;

		byte cmd = animCmds[cmdIdx];
		byte cmdCode = cmd & 0x7F;

		if (cmdCode >= 0x71 && cmdCode <= 0x7C)
			continue;

		PictureInfo pic = getPictureInfo(data, size, format, limbIdx, cmdCode);
		if (!pic.valid)
			continue;

		// Calculate bounding box for this picture
		int picMinX = pic.relX;
		int picMinY = pic.relY;
		int picMaxX = pic.relX + pic.width;
		int picMaxY = pic.relY + pic.height;

		if (!dims.valid) {
			dims.minX = picMinX;
			dims.minY = picMinY;
			dims.maxX = picMaxX;
			dims.maxY = picMaxY;
			dims.valid = true;
		} else {
			dims.minX = MIN(dims.minX, picMinX);
			dims.minY = MIN(dims.minY, picMinY);
			dims.maxX = MAX(dims.maxX, picMaxX);
			dims.maxY = MAX(dims.maxY, picMaxY);
		}
	}

	return dims;
}

LimbDimensions Costume::getAnimationDimensions(const byte *data, uint32 size, byte format, int numColors, int animIndex) {
	LimbDimensions animDims;

	// Calculate offsets
	int animOffsetsPos = 8 + numColors + 2 + 32;
	uint16 animCmdsOffset = READ_LE_UINT16(data + 8 + numColors);

	// Get animation offset
	uint16 animOffset = READ_LE_UINT16(data + animOffsetsPos + animIndex * 2);
	if (animOffset == 0 || (uint32)animOffset + 2 > size)
		return animDims;

	const byte *animData = data + animOffset;
	uint16 limbMask = READ_LE_UINT16(animData);

	// Iterate through all active limbs
	const byte *limbDataPtr = animData + 2;
	uint32 remainingSize = size - (animOffset + 2);

	for (int limb = 0; limb < 16 && remainingSize >= 2; ++limb) {
		bool isActive = (limbMask & (1 << (15 - limb))) != 0;
		if (!isActive)
			continue;

		uint16 startOffset = READ_LE_UINT16(limbDataPtr);
		limbDataPtr += 2;
		remainingSize -= 2;

		// Skip disabled limbs
		if (startOffset == 0xFFFF)
			continue;

		if (remainingSize < 1)
			break;

		byte extra = *limbDataPtr;
		limbDataPtr += 1;
		remainingSize -= 1;

		uint16 endOffset = startOffset + (extra & 0x7F);

		// Get dimensions for this limb
		LimbDimensions limbDims = getLimbMaxDimensions(data, size, format, limb, startOffset, endOffset, animCmdsOffset);
		if (!limbDims.valid)
			continue;

		// Combine with animation dimensions
		if (!animDims.valid) {
			animDims = limbDims;
		} else {
			animDims.minX = MIN(animDims.minX, limbDims.minX);
			animDims.minY = MIN(animDims.minY, limbDims.minY);
			animDims.maxX = MAX(animDims.maxX, limbDims.maxX);
			animDims.maxY = MAX(animDims.maxY, limbDims.maxY);
		}
	}

	return animDims;
}

void Costume::initAnimState(const byte *data, uint32 size, byte format, int numColors, int animIndex) {
	// Reset all limbs
	for (int i = 0; i < 16; i++) {
		_animCurPos[i] = 0xFFFF;
		_animStart[i] = 0;
		_animEnd[i] = 0;
		_animLooping[i] = false;
	}
	_animCounter = 0;
	_animMaxCounter = 0;

	// Get animation data
	int animOffsetsPos = 8 + numColors + 2 + 32;
	uint16 animOffset = READ_LE_UINT16(data + animOffsetsPos + animIndex * 2);
	if (animOffset == 0 || (uint32)animOffset + 2 > size)
		return;

	const byte *animData = data + animOffset;
	uint16 limbMask = READ_LE_UINT16(animData);
	const byte *limbDataPtr = animData + 2;
	uint32 remainingSize = size - (animOffset + 2);

	int maxLength = 0;

	for (int limb = 0; limb < 16 && remainingSize >= 2; ++limb) {
		bool isActive = (limbMask & (1 << (15 - limb))) != 0;
		if (!isActive)
			continue;

		uint16 startOffset = READ_LE_UINT16(limbDataPtr);
		limbDataPtr += 2;
		remainingSize -= 2;

		if (startOffset == 0xFFFF)
			continue;

		if (remainingSize < 1)
			break;

		byte extra = *limbDataPtr;
		limbDataPtr += 1;
		remainingSize -= 1;

		uint16 endOffset = startOffset + (extra & 0x7F);
		bool loop = (extra & 0x80) != 0;

		_animCurPos[limb] = startOffset;
		_animStart[limb] = startOffset;
		_animEnd[limb] = endOffset;
		_animLooping[limb] = loop;

		// Track max length for the animation counter
		int limbLength = endOffset - startOffset + 1;
		maxLength = MAX(maxLength, limbLength);
	}

	_animMaxCounter = maxLength;

	// Update selected command if a limb is selected
	if (_selectedLimb >= 0 && _animCurPos[_selectedLimb] != 0xFFFF) {
		_selectedCommand = _animCurPos[_selectedLimb];
	}
}

void Costume::stepAnimForward(const byte *data, uint32 size, uint16 animCmdsOffset) {
	if ((uint32)animCmdsOffset >= size)
		return;

	const byte *animCmds = data + animCmdsOffset;

	for (int limb = 0; limb < 16; ++limb) {
		if (_animCurPos[limb] == 0xFFFF)
			continue;

		uint16 curPos = _animCurPos[limb];
		uint16 start = _animStart[limb];
		uint16 end = _animEnd[limb];

		// Advance position (looping wraps around, non-looping stops at end)
		if (_animLooping[limb]) {
			curPos = (curPos >= end) ? start : curPos + 1;
		} else {
			if (curPos < end)
				curPos++;
		}

		// Skip SOUND/DELAY commands, stop on PIC/STOP/START/HIDE
		while ((uint32)animCmdsOffset + curPos < size) {
			byte cmd = animCmds[curPos] & 0x7F;
			if (cmd < 0x71)
				break;
			if (cmd >= 0x79 && cmd <= 0x7B)
				break;
			if (_animLooping[limb] && curPos >= end) {
				curPos = start;
			} else if (curPos < end) {
				curPos++;
			} else {
				break;
			}
		}

		_animCurPos[limb] = curPos;
	}

	// Update counter and reset limbs when wrapping
	_animCounter++;
	if (_animMaxCounter > 0 && _animCounter >= _animMaxCounter) {
		_animCounter = 0;
		for (int limb = 0; limb < 16; ++limb) {
			if (_animCurPos[limb] != 0xFFFF)
				_animCurPos[limb] = _animStart[limb];
		}
	}

	if (_selectedLimb >= 0 && _animCurPos[_selectedLimb] != 0xFFFF)
		_selectedCommand = _animCurPos[_selectedLimb];
}

void Costume::stepAnimBackward(const byte *data, uint32 size, uint16 animCmdsOffset) {
	if ((uint32)animCmdsOffset >= size)
		return;

	const byte *animCmds = data + animCmdsOffset;

	for (int limb = 0; limb < 16; ++limb) {
		if (_animCurPos[limb] == 0xFFFF)
			continue;

		uint16 curPos = _animCurPos[limb];
		uint16 start = _animStart[limb];
		uint16 end = _animEnd[limb];

		// Go back (looping wraps around, non-looping stops at start)
		if (_animLooping[limb]) {
			curPos = (curPos <= start) ? end : curPos - 1;
		} else {
			if (curPos > start)
				curPos--;
		}

		// Skip SOUND/DELAY commands, stop on PIC/STOP/START/HIDE
		while ((uint32)animCmdsOffset + curPos < size) {
			byte cmd = animCmds[curPos] & 0x7F;
			if (cmd < 0x71)
				break;
			if (cmd >= 0x79 && cmd <= 0x7B)
				break;
			if (_animLooping[limb] && curPos <= start) {
				curPos = end;
			} else if (curPos > start) {
				curPos--;
			} else {
				break;
			}
		}

		_animCurPos[limb] = curPos;
	}

	// Update counter and reset limbs when wrapping
	_animCounter--;
	if (_animCounter < 0) {
		_animCounter = (_animMaxCounter > 0) ? _animMaxCounter - 1 : 0;
		for (int limb = 0; limb < 16; ++limb) {
			if (_animCurPos[limb] != 0xFFFF)
				_animCurPos[limb] = _animEnd[limb];
		}
	}

	if (_selectedLimb >= 0 && _animCurPos[_selectedLimb] != 0xFFFF)
		_selectedCommand = _animCurPos[_selectedLimb];
}

void Costume::seekAnimTo(const byte *data, uint32 size, byte format, int numColors, int animIndex, int targetCounter) {
	initAnimState(data, size, format, numColors, animIndex);
	uint16 animCmdsOffset = READ_LE_UINT16(data + 8 + numColors);
	for (int i = 0; i < targetCounter && i < _animMaxCounter; ++i) {
		stepAnimForward(data, size, animCmdsOffset);
	}
}

byte Costume::getLimbPicture(int limb, const byte *animCmds) const {
	if (limb < 0 || limb >= 16 || _animCurPos[limb] == 0xFFFF)
		return 0xFF;
	return animCmds[_animCurPos[limb]] & 0x7F;
}

void Costume::decodePicture(ImDrawList *drawList, ImVec2 pos, int scale, const PictureInfo &pic, byte format, const Common::Array<byte> &costumePalette, const Common::Array<Color> *roomColors, bool mirror, bool transparent) {
	if (!pic.valid || !pic.rleData)
		return;

	int shift = (format & 0x01) ? 3 : 4;
	int rleMask = (format & 0x01) ? 7 : 15;

	int x = 0;
	int y = 0;
	uint32 rlePos = 0;

	while (x < pic.width && rlePos < pic.rleSize) {
		byte cmd = pic.rleData[rlePos++];
		byte colorIdx = cmd >> shift;
		int rep = cmd & rleMask;

		if (rep == 0) {
			if (rlePos >= pic.rleSize)
				break;
			rep = pic.rleData[rlePos++];
		}

		// Resolve color through costume and room palettes
		ImU32 pixelColor = IM_COL32(128, 128, 128, 255);
		if (roomColors && !costumePalette.empty() && colorIdx < costumePalette.size()) {
			byte roomPaletteIdx = costumePalette[colorIdx];
			if (roomPaletteIdx < roomColors->size()) {
				const Color &c = (*roomColors)[roomPaletteIdx];
				pixelColor = IM_COL32(c.r, c.g, c.b, 255);
			}
		}

		// Draw pixels
		while (rep > 0 && x < pic.width) {
			if (!(transparent && colorIdx == 0)) {
				int drawX = mirror ? (pic.width - 1 - x) : x;
				ImVec2 p1(pos.x + drawX * scale, pos.y + y * scale);
				ImVec2 p2(p1.x + scale, p1.y + scale);
				drawList->AddRectFilled(p1, p2, pixelColor);
			}

			y++;
			rep--;

			if (y >= pic.height) {
				y = 0;
				x++;
			}
		}
	}
}

static ImU32 getBackgroundColor(const Common::Array<byte> &costumePalette, const Common::Array<Color> *roomColors) {
	if (roomColors && !costumePalette.empty()) {
		byte roomPaletteIdx = costumePalette[0];
		if (roomPaletteIdx < roomColors->size()) {
			const Color &c = (*roomColors)[roomPaletteIdx];
			return IM_COL32(c.r, c.g, c.b, 255);
		}
	}
	return IM_COL32(0, 0, 0, 255);
}

void Costume::renderPicturePreview(const PictureInfo &pic, byte format, const Common::Array<byte> &costumePalette, const LimbDimensions &limbDims, const Common::Array<Color> *roomColors) {
	if (!pic.valid || !limbDims.valid)
		return;

	ImGui::SeparatorText("Picture Preview");

	// Scale control, transparency, and bounds toggle
	static int picScale = 2;
	static bool picTransparent = true;
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("Scale", &picScale, 1, 8, "%dx");
	ImGui::SameLine();
	ImGui::Checkbox("Transparent", &picTransparent);
	ImGui::SameLine();
	ImGui::Checkbox("Bounds", &_showBoundingBox);

	int scale = picScale;
	int outerWidth = limbDims.width();
	int outerHeight = limbDims.height();

	if (outerWidth <= 0 || outerHeight <= 0)
		return;

	ImGui::BeginChild("PicPreviewScroll", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	ImVec2 outerP1 = canvasPos;
	ImVec2 outerP2(canvasPos.x + outerWidth * scale, canvasPos.y + outerHeight * scale);

	// Draw background
	if (!picTransparent)
		drawList->AddRectFilled(outerP1, outerP2, getBackgroundColor(costumePalette, roomColors));

	// Position and draw picture
	int picX = (pic.relX - limbDims.minX) * scale;
	int picY = (pic.relY - limbDims.minY) * scale;
	ImVec2 picPos(canvasPos.x + picX, canvasPos.y + picY);
	decodePicture(drawList, picPos, scale, pic, format, costumePalette, roomColors, false, picTransparent);

	// Draw bounding box
	if (_showBoundingBox) {
		ImVec2 boxP1 = picPos;
		ImVec2 boxP2(picPos.x + pic.width * scale, picPos.y + pic.height * scale);
		drawList->AddRect(boxP1, boxP2, IM_COL32(255, 255, 0, 255), 0.0f, 0, 1.0f);
	}

	// Reserve space for the drawing
	ImGui::Dummy(ImVec2((float)outerWidth * scale, (float)outerHeight * scale));

	ImGui::EndChild();
}

bool Costume::renderAnimFirstFrame(const byte *data, uint32 size, byte format, int numColors, int animIndex, const Common::Array<byte> &costumePalette, const Common::Array<Color> *roomColors) {
	// Validate animation dimensions
	LimbDimensions animDims = getAnimationDimensions(data, size, format, numColors, animIndex);
	if (!animDims.valid)
		return false;

	int previewWidth = animDims.width();
	int previewHeight = animDims.height();
	if (previewWidth <= 0 || previewHeight <= 0)
		return false;

	// Get animation data
	int animOffsetsPos = 8 + numColors + 2 + 32;
	uint16 animCmdsOffset = READ_LE_UINT16(data + 8 + numColors);
	uint16 animOffset = READ_LE_UINT16(data + animOffsetsPos + animIndex * 2);
	if (animOffset == 0 || (uint32)animOffset + 2 > size)
		return false;

	const byte *animData = data + animOffset;
	uint16 limbMask = READ_LE_UINT16(animData);
	const byte *animCmds = data + animCmdsOffset;

	// Check for valid PIC command
	bool hasPic = false;
	const byte *scanPtr = animData + 2;
	uint32 scanRemaining = size - (animOffset + 2);

	for (int limb = 0; limb < 16 && scanRemaining >= 2; ++limb) {
		bool isActive = (limbMask & (1 << (15 - limb))) != 0;
		if (!isActive)
			continue;

		uint16 startOffset = READ_LE_UINT16(scanPtr);
		scanPtr += 2;
		scanRemaining -= 2;

		if (startOffset == 0xFFFF)
			continue;

		if (scanRemaining < 1)
			break;
		scanPtr += 1;
		scanRemaining -= 1;

		if ((uint32)animCmdsOffset + startOffset < size) {
			byte cmdCode = animCmds[startOffset] & 0x7F;
			if (cmdCode < 0x71) {
				PictureInfo pic = getPictureInfo(data, size, format, limb, cmdCode);
				if (pic.valid) {
					hasPic = true;
					break;
				}
			}
		}
	}

	if (!hasPic)
		return false;

	// Draw at 1:1 scale
	int scale = 1;
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	const byte *limbDataPtr = animData + 2;
	uint32 remainingSize = size - (animOffset + 2);

	for (int limb = 0; limb < 16 && remainingSize >= 2; ++limb) {
		bool isActive = (limbMask & (1 << (15 - limb))) != 0;
		if (!isActive)
			continue;

		uint16 startOffset = READ_LE_UINT16(limbDataPtr);
		limbDataPtr += 2;
		remainingSize -= 2;

		if (startOffset == 0xFFFF)
			continue;

		if (remainingSize < 1)
			break;

		limbDataPtr += 1;
		remainingSize -= 1;

		if ((uint32)animCmdsOffset + startOffset >= size)
			continue;

		byte cmd = animCmds[startOffset];
		byte cmdCode = cmd & 0x7F;
		if (cmdCode >= 0x71)
			continue;

		PictureInfo pic = getPictureInfo(data, size, format, limb, cmdCode);
		if (!pic.valid)
			continue;

		int picX = (pic.relX - animDims.minX) * scale;
		int picY = (pic.relY - animDims.minY) * scale;
		ImVec2 picPos(canvasPos.x + picX, canvasPos.y + picY);

		decodePicture(drawList, picPos, scale, pic, format, costumePalette, roomColors, false, true);
	}

	// Reserve space
	ImGui::Dummy(ImVec2((float)previewWidth * scale, (float)previewHeight * scale));
	return true;
}

void Costume::renderCostumeTooltip(int costumeId) {
	COST *cost = findCostume(costumeId);
	if (!cost || cost->data.size() < 8)
		return;

	const byte *data = cost->data.data();
	uint32 size = cost->data.size();

	uint32 costumeSize = READ_LE_UINT32(data);
	byte numAnims = data[6];
	if (costumeSize == 0)
		numAnims++;
	byte format = data[7];
	int numColors = (format & 0x01) ? 32 : 16;

	// Need at least anim index 4 (init west)
	if (numAnims <= 4)
		return;

	// Pre-validate dimensions before opening tooltip
	LimbDimensions animDims = getAnimationDimensions(data, size, format, numColors, 4);
	if (!animDims.valid || animDims.width() <= 0 || animDims.height() <= 0)
		return;

	Common::Array<byte> costumePalette = getCostumePalette(data, size);

	LFLF *lflf = findLFLF(costumeId);
	const Common::Array<Color> *roomColors = nullptr;
	if (lflf && !lflf->room.pals.wrap.apals.empty()) {
		roomColors = &lflf->room.pals.wrap.apals[0].colors;
	}

	ImGui::BeginTooltip();
	renderAnimFirstFrame(data, size, format, numColors, 4, costumePalette, roomColors);
	ImGui::EndTooltip();
}

void Costume::renderAnimTooltip(int animGroup) {
	if (_selectedCostume < 0)
		return;

	COST *cost = findCostume(_selectedCostume);
	if (!cost || cost->data.size() < 8)
		return;

	const byte *data = cost->data.data();
	uint32 size = cost->data.size();

	uint32 costumeSize = READ_LE_UINT32(data);
	byte numAnims = data[6];
	if (costumeSize == 0)
		numAnims++;
	byte format = data[7];
	int numColors = (format & 0x01) ? 32 : 16;

	// West direction of this group
	int animIndex = animGroup * 4;
	if (animIndex >= numAnims)
		return;

	// Pre-validate dimensions before opening tooltip
	LimbDimensions animDims = getAnimationDimensions(data, size, format, numColors, animIndex);
	if (!animDims.valid || animDims.width() <= 0 || animDims.height() <= 0)
		return;

	Common::Array<byte> costumePalette = getCostumePalette(data, size);

	const Common::Array<Color> *roomColors = getRoomColors();

	ImGui::BeginTooltip();
	renderAnimFirstFrame(data, size, format, numColors, animIndex, costumePalette, roomColors);
	ImGui::EndTooltip();
}

void Costume::renderCostumeList() {
	ImGui::BeginChild("CostumeList", ImVec2(200, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Costumes");

	// Build a map of room -> costumes
	Common::HashMap<byte, Common::Array<int>> roomCostumes;
	for (uint i = 0; i < _dcos.locations.size(); ++i) {
		byte roomNum = _dcos.locations[i];
		if (roomNum == 0)
			continue;
		roomCostumes[roomNum].push_back((int)i);
	}

	// Render as a tree of rooms
	for (auto &entry : roomCostumes) {
		byte roomNum = entry._key;
		const Common::Array<int> &costumes = entry._value;

		char roomLabel[64];
		Common::sprintf_s(roomLabel, "Room %d", roomNum);

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::TreeNodeEx(roomLabel, nodeFlags)) {
			for (uint j = 0; j < costumes.size(); ++j) {
				int costumeId = costumes[j];
				char costumeLabel[64];
				Common::sprintf_s(costumeLabel, "Costume %d", costumeId);
				if (ImGui::Selectable(costumeLabel, _selectedCostume == costumeId)) {
					_selectedCostume = costumeId;
					_selectedPaletteRoom = -1;
					_selectedAnimGroup = -1;
					_selectedDirection = DIR_WEST;
					_selectedLimb = -1;
					_selectedCommand = -1;

					// Auto-select init animation if present
					COST *cost = findCostume(_selectedCostume);
					if (cost && cost->data.size() >= 8) {
						const byte *data = cost->data.data();
						uint32 costumeSize = READ_LE_UINT32(data);
						byte numAnims = data[6];
						if (costumeSize == 0)
							numAnims++;
						if (numAnims > 4) {
							_selectedAnimGroup = 1;
							_selectedDirection = DIR_WEST;
						}
					}
				}
				if (ImGui::IsItemHovered()) {
					renderCostumeTooltip(costumeId);
				}
			}
			ImGui::TreePop();
		}
	}

	ImGui::EndChild();
}

void Costume::renderCostumeProperties() {
	ImGui::BeginChild("CostumeProperties", ImVec2(600, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

	if (_selectedCostume < 0) {
		ImGui::TextColored(SOL_BASE01, "Select a costume.");
		ImGui::EndChild();
		return;
	}

	COST *cost = findCostume(_selectedCostume);
	if (!cost || cost->data.size() < 8) {
		ImGui::TextColored(SOL_BASE01, "Costume data not found.");
		ImGui::EndChild();
		return;
	}

	const byte *data = cost->data.data();
	uint32 costumeSize = READ_LE_UINT32(data);
	char header[3] = { (char)data[4], (char)data[5], '\0' };
	byte numAnims = data[6];
	if (costumeSize == 0)
		numAnims++;
	byte format = data[7];
	bool mirror = (format & 0x80) != 0;
	int numColors = (format & 0x01) ? 32 : 16;

	char headerBuf[64];
	Common::sprintf_s(headerBuf, "Costume %d", _selectedCostume);
	ImGui::SeparatorText(headerBuf);

	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("CostumeHeader", 2, flags)) {
		ImGui::TableSetupColumn("Property");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Size");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%u", costumeSize);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Header");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%s", header);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Num Anims");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", numAnims);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Mirror");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%s", mirror ? "Yes" : "No");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BLUE, "Num Colors");
		ImGui::TableNextColumn();
		ImGui::TextColored(SOL_BASE0, "%d", numColors);

		ImGui::EndTable();
	}

	if (ImGui::BeginTabBar("CostumePropsTabs")) {
		if (ImGui::BeginTabItem("Palette")) {
			renderPaletteTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animations")) {
			renderAnimationsTab();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::EndChild();
}

void Costume::renderPaletteTab() {
	if (_selectedCostume < 0) {
		ImGui::TextColored(SOL_BASE01, "Select a costume.");
		return;
	}

	COST *cost = findCostume(_selectedCostume);
	if (!cost || cost->data.size() < 8) {
		ImGui::TextColored(SOL_BASE01, "No palette data..");
		return;
	}

	Common::Array<byte> costumePalette = getCostumePalette(cost->data.data(), cost->data.size());
	if (costumePalette.empty()) {
		ImGui::TextColored(SOL_BASE01, "No palette data.");
		return;
	}

	byte costumeRoom = _dcos.locations[_selectedCostume];

	ImGui::SeparatorText("Room Palette Source");

	int currentRoom = (_selectedPaletteRoom < 0) ? costumeRoom : _selectedPaletteRoom;
	char comboLabel[64];
	Common::sprintf_s(comboLabel, "Room %d%s", currentRoom, (_selectedPaletteRoom < 0) ? " (costume's room)" : "");

	if (ImGui::BeginCombo("Room", comboLabel)) {
		char defaultLabel[64];
		Common::sprintf_s(defaultLabel, "Room %d (costume's room)", costumeRoom);
		if (ImGui::Selectable(defaultLabel, _selectedPaletteRoom < 0)) {
			_selectedPaletteRoom = -1;
		}

		for (uint i = 0; i < _lecf.loff.locations.size(); ++i) {
			if (_lecf.loff.locations[i] == 0)
				continue;
			if (_lecf.lflfs[i].room.pals.wrap.apals.empty())
				continue;

			int roomNum = _lecf.loff.locations[i];
			if (roomNum == costumeRoom)
				continue;

			char label[64];
			Common::sprintf_s(label, "Room %d", roomNum);
			if (ImGui::Selectable(label, _selectedPaletteRoom == roomNum)) {
				_selectedPaletteRoom = roomNum;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SeparatorText("Costume Palette");
	ImGui::TextColored(SOL_BASE01, "(%d entries, mapped to room palette)", (int)costumePalette.size());

	LFLF *lflf = (_selectedPaletteRoom < 0) ? findLFLF(_selectedCostume) : findLFLFByRoom(_selectedPaletteRoom);
	if (!lflf || lflf->room.pals.wrap.apals.empty()) {
		ImGui::TextColored(SOL_RED, "Room palette not found.");
		return;
	}

	const Common::Array<Color> &roomColors = lflf->room.pals.wrap.apals[0].colors;

	ImGui::BeginChild("CostumePaletteGrid", ImVec2(0, 0), ImGuiChildFlags_Borders);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	for (int i = 0; i < (int)costumePalette.size(); ++i) {
		ImGui::PushID(i);

		byte colorIndex = costumePalette[i];
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

void Costume::renderLimbProperties(const byte *data, uint32 size, byte format, int numColors, uint16 limbMask, uint16 animCmdsOffset) {
	bool limbActive = (limbMask & (1 << (15 - _selectedLimb))) != 0;

	char headerBuf[64];
	Common::sprintf_s(headerBuf, "Limb %d Properties", _selectedLimb);
	ImGui::SeparatorText(headerBuf);

	if (!limbActive) {
		ImGui::TextColored(SOL_BASE01, "Limb not used in this animation.");
		return;
	}

	// Calculate animation data position
	COST *cost = findCostume(_selectedCostume);
	if (!cost)
		return;

	uint32 costumeSize = READ_LE_UINT32(data);
	byte numAnims = data[6];
	if (costumeSize == 0)
		numAnims++;

	int animOffsetsPos = 8 + numColors + 2 + 32;
	int animIndex = _selectedAnimGroup * 4 + _selectedDirection;

	if (animIndex >= numAnims)
		return;

	uint16 animOffset = READ_LE_UINT16(data + animOffsetsPos + animIndex * 2);
	if (animOffset == 0 || (uint32)animOffset + 2 > size)
		return;

	const byte *animData = data + animOffset;
	const byte *limbDataPtr = animData + 2;
	uint32 remainingSize = size - (animOffset + 2);

	// Find the data for the selected limb
	for (int i = 0; i <= _selectedLimb && remainingSize >= 2; ++i) {
		bool isActive = (limbMask & (1 << (15 - i))) != 0;
		if (!isActive)
			continue;

		uint16 startOffset = READ_LE_UINT16(limbDataPtr);

		if (i == _selectedLimb) {
			if (startOffset == 0xFFFF) {
				ImGui::TextColored(SOL_YELLOW, "Limb is disabled.");
				return;
			}

			limbDataPtr += 2;
			remainingSize -= 2;
			if (remainingSize < 1)
				return;

			byte extra = *limbDataPtr;
			uint16 endOffset = startOffset + (extra & 0x7F);
			bool loop = (extra & 0x80) != 0;

			const ImGuiTableFlags tblFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			if (ImGui::BeginTable("LimbProps", 2, tblFlags)) {
				ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Value");

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BLUE, "Start");
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "0x%04X (%d)", startOffset, startOffset);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BLUE, "End");
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "0x%04X (%d)", endOffset, endOffset);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BLUE, "Loop");
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%s", loop ? "Yes" : "No");

				ImGui::EndTable();
			}

			// Calculate limb max dimensions
			LimbDimensions limbDims = getLimbMaxDimensions(data, size, format, _selectedLimb, startOffset, endOffset, animCmdsOffset);

			// Render command list
			ImGui::Spacing();
			renderCommandList(data, size, format, numColors, startOffset, endOffset, animCmdsOffset, limbDims);
			return;
		}

		// Skip this limb's data
		limbDataPtr += 2;
		remainingSize -= 2;
		if (startOffset != 0xFFFF && remainingSize >= 1) {
			limbDataPtr += 1;
			remainingSize -= 1;
		}
	}

	ImGui::TextColored(SOL_RED, "Failed to parse limb data.");
}

void Costume::renderCommandList(const byte *data, uint32 size, byte format, int numColors, uint16 startOffset, uint16 endOffset, uint16 animCmdsOffset, const LimbDimensions &limbDims) {
	if ((uint32)animCmdsOffset >= size)
		return;

	const byte *animCmds = data + animCmdsOffset;
	byte selectedCmdCode = 0;
	bool selectedIsPic = false;

	// Left side: Command list
	ImGui::BeginChild("CmdListPanel", ImVec2(180, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Commands");

	for (uint16 cmdIdx = startOffset; cmdIdx <= endOffset; ++cmdIdx) {
		if ((uint32)animCmdsOffset + cmdIdx >= size)
			break;

		byte cmd = animCmds[cmdIdx];
		byte cmdCode = cmd & 0x7F;

		const char *cmdDesc;
		bool isPic = false;
		if (cmdCode >= 0x71 && cmdCode <= 0x78) {
			cmdDesc = "SOUND";
		} else if (cmdCode == 0x79) {
			cmdDesc = "STOP";
		} else if (cmdCode == 0x7A) {
			cmdDesc = "START";
		} else if (cmdCode == 0x7B) {
			cmdDesc = "HIDE";
		} else if (cmdCode == 0x7C) {
			cmdDesc = "SKIP";
		} else {
			cmdDesc = nullptr;
			isPic = true;
		}

		char label[64];
		if (cmdDesc) {
			Common::sprintf_s(label, "[%d] 0x%02X: %s", cmdIdx, cmd, cmdDesc);
		} else {
			Common::sprintf_s(label, "[%d] 0x%02X: PIC %d", cmdIdx, cmd, cmdCode);
		}

		if (ImGui::Selectable(label, _selectedCommand == cmdIdx)) {
			_selectedCommand = cmdIdx;
		}

		if (_selectedCommand == cmdIdx) {
			selectedCmdCode = cmdCode;
			selectedIsPic = isPic;
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	// Right side: Picture properties and preview
	ImGui::BeginChild("PicPropsPanel", ImVec2(0, 0), ImGuiChildFlags_Borders);

	if (_selectedCommand >= 0 && selectedIsPic) {
		char headerBuf[64];
		Common::sprintf_s(headerBuf, "Picture %d Properties", selectedCmdCode);
		ImGui::SeparatorText(headerBuf);

		PictureInfo pic = getPictureInfo(data, size, format, _selectedLimb, selectedCmdCode);

		if (pic.valid) {
			bool hasRedir = (format & 0x7E) == 0x60;
			const ImGuiTableFlags tblFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			if (ImGui::BeginTable("PicDims", 2, tblFlags)) {
				ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 80);
				ImGui::TableSetupColumn("Value");

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BLUE, "Width");
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", pic.width);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BLUE, "Height");
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", pic.height);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BLUE, "Rel X");
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", pic.relX);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BLUE, "Rel Y");
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", pic.relY);

				if (hasRedir) {
					ImVec4 limbColor = (pic.redirLimb == 0xFF) ? SOL_BASE01 : SOL_BASE0;
					ImVec4 pictColor = (pic.redirPict == 0xFF) ? SOL_BASE01 : SOL_BASE0;

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BLUE, "Redir Limb");
					ImGui::TableNextColumn();
					if (pic.redirLimb == 0xFF) {
						ImGui::TextColored(limbColor, "None");
					} else {
						ImGui::TextColored(limbColor, "%d", pic.redirLimb);
					}

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(SOL_BLUE, "Redir Pict");
					ImGui::TableNextColumn();
					if (pic.redirPict == 0xFF) {
						ImGui::TextColored(pictColor, "None");
					} else {
						ImGui::TextColored(pictColor, "%d", pic.redirPict);
					}
				}

				ImGui::EndTable();
			}

			// Get room palette and costume palette
			const Common::Array<Color> *roomColors = getRoomColors();
			Common::Array<byte> costumePalette = getCostumePalette(data, size);

			// Render picture preview
			renderPicturePreview(pic, format, costumePalette, limbDims, roomColors);
		} else {
			ImGui::TextColored(SOL_RED, "Invalid picture data.");
		}
	} else if (_selectedCommand >= 0) {
		ImGui::TextColored(SOL_BASE01, "Selected command is not a picture.");
	} else {
		ImGui::TextColored(SOL_BASE01, "Select a command.");
	}

	ImGui::EndChild();
}

void Costume::renderAnimationsTab() {
	if (_selectedCostume < 0) {
		ImGui::TextColored(SOL_BASE01, "Select a costume.");
		return;
	}

	COST *cost = findCostume(_selectedCostume);
	if (!cost || cost->data.size() < 8) {
		ImGui::TextColored(SOL_BASE01, "Costume data not found.");
		return;
	}

	const byte *data = cost->data.data();
	uint32 size = cost->data.size();
	uint32 costumeSize = READ_LE_UINT32(data);
	byte numAnims = data[6];
	if (costumeSize == 0)
		numAnims++;
	byte format = data[7];
	int numColors = (format & 0x01) ? 32 : 16;
	int animOffsetsPos = 8 + numColors + 2 + 32;

	if ((uint32)animOffsetsPos + numAnims * 2 > size) {
		ImGui::TextColored(SOL_RED, "Invalid costume data.");
		return;
	}

	uint16 animCmdsOffset = READ_LE_UINT16(data + 8 + numColors);
	int numGroups = (numAnims + 3) / 4;

	// Animation group list
	ImGui::BeginChild("AnimGroupList", ImVec2(180, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

	ImGui::SeparatorText("Animation Groups");

	for (int group = 0; group < numGroups; ++group) {
		int startAnim = group * 4;
		int endAnim = MIN(startAnim + 3, (int)numAnims - 1);

		char label[64];
		if (group < numStandardGroups) {
			Common::sprintf_s(label, "Anim %d-%d (%s)", startAnim, endAnim, animGroupNames[group]);
		} else {
			Common::sprintf_s(label, "Anim %d-%d", startAnim, endAnim);
		}

		if (ImGui::Selectable(label, _selectedAnimGroup == group)) {
			_selectedAnimGroup = group;
			_selectedLimb = -1;
			_selectedCommand = -1;
		}
		if (ImGui::IsItemHovered()) {
			renderAnimTooltip(group);
		}
	}

	ImGui::EndChild();
	ImGui::SameLine();

	// Animation properties panel
	ImGui::BeginChild("AnimProperties", ImVec2(0, 0), ImGuiChildFlags_Borders);
	if (_selectedAnimGroup < 0) {
		ImGui::TextColored(SOL_BASE01, "Select an animation group.");
		ImGui::EndChild();
		return;
	}

	int baseAnim = _selectedAnimGroup * 4;
	int maxAnimInGroup = MIN(3, (int)numAnims - 1 - baseAnim);

	// Playback controls
	ImGui::SeparatorText("Playback");

	float iconScale = 2.0f;
	ImGui::SetWindowFontScale(iconScale);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));

	// Re-initialize if selection changed
	int animIndex = baseAnim + _selectedDirection;
	if (_lastCostume != _selectedCostume || _lastAnimGroup != _selectedAnimGroup || _lastDirection != (int)_selectedDirection) {
		_lastCostume = _selectedCostume;
		_lastAnimGroup = _selectedAnimGroup;
		_lastDirection = (int)_selectedDirection;
		_animPlaying = false;
		initAnimState(data, size, format, numColors, animIndex);
	}

	// Step backward
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.165f, 0.631f, 0.596f, 1.0f));
	if (ImGui::Button(ICON_STEP_BACK "##stepback")) {
		stepAnimBackward(data, size, animCmdsOffset);
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Step Backward");

	ImGui::SameLine(0, 4);

	// Auto-step when playing (60 FPS tick rate)
	if (_animPlaying && _animMaxCounter > 0) {
		_animFrameAccum += ImGui::GetIO().DeltaTime;
		const float frameTime = 1.0f / 60.0f;
		while (_animFrameAccum >= frameTime) {
			_animFrameAccum -= frameTime;
			_animProgress++;
			if (_animProgress >= _animSpeed) {
				_animProgress = 0;
				stepAnimForward(data, size, animCmdsOffset);
			}
		}
	}

	// Play
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.85f, 0.0f, 1.0f));
	if (ImGui::Button(ICON_PLAY "##play")) {
		_animPlaying = true;
		_animProgress = 0;
		_animFrameAccum = 0.0f;
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Play");
	ImGui::SameLine(0, 4);

	// Pause
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.710f, 0.537f, 0.000f, 1.0f));
	if (ImGui::Button(ICON_PAUSE "##pause"))
		_animPlaying = false;
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Pause");
	ImGui::SameLine(0, 4);

	// Stop
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.863f, 0.196f, 0.184f, 1.0f));
	if (ImGui::Button(ICON_STOP "##stop")) {
		_animPlaying = false;
		initAnimState(data, size, format, numColors, animIndex);
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Stop");
	ImGui::SameLine(0, 4);

	// Step forward
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.165f, 0.631f, 0.596f, 1.0f));
	if (ImGui::Button(ICON_STEP_FWD "##stepfwd")) {
		stepAnimForward(data, size, animCmdsOffset);
	}
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Step Forward");

	ImGui::PopStyleColor(3);
	ImGui::SetWindowFontScale(1.0f);

	// Frame counter slider
	if (_animMaxCounter > 0) {
		ImGui::SetNextItemWidth(332);
		int counter = _animCounter;
		if (ImGui::SliderInt("##counter", &counter, 0, _animMaxCounter - 1, "%d")) {
			if (counter != _animCounter) {
				seekAnimTo(data, size, format, numColors, animIndex, counter);
			}
		}
		ImGui::SameLine();
		ImGui::TextColored(SOL_BASE01, "/ %d", _animMaxCounter - 1);
	}

	// Speed control
	ImGui::SetNextItemWidth(70);
	static const char *speedLabels[] = { "60", "30", "20", "15", "12", "10", "8" };
	static const byte speedValues[] = { 0, 2, 3, 4, 5, 6, 7 };
	int speedIdx = 4; // Default to 12 FPS (speed 5)
	for (int i = 0; i < 7; ++i) {
		if (speedValues[i] == _animSpeed) {
			speedIdx = i;
			break;
		}
	}
	char speedPreview[16];
	Common::sprintf_s(speedPreview, "%s FPS", speedLabels[speedIdx]);
	if (ImGui::BeginCombo("##speed", speedPreview, ImGuiComboFlags_HeightSmall)) {
		for (int i = 0; i < 7; ++i) {
			char label[16];
			Common::sprintf_s(label, "%s FPS", speedLabels[i]);
			if (ImGui::Selectable(label, speedIdx == i)) {
				_animSpeed = speedValues[i];
			}
		}
		ImGui::EndCombo();
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Animation speed");

	// Direction selection
	ImGui::SeparatorText("Direction");

	for (int dir = 0; dir <= maxAnimInGroup; ++dir) {
		if (ImGui::RadioButton(directionNames[dir], _selectedDirection == (Direction)dir)) {
			_selectedDirection = (Direction)dir;
		}
		if (dir < maxAnimInGroup)
			ImGui::SameLine();
	}

	// Validate selected animation
	if (animIndex >= numAnims) {
		ImGui::TextColored(SOL_BASE01, "Animation not available.");
		ImGui::EndChild();
		return;
	}

	uint16 animOffset = READ_LE_UINT16(data + animOffsetsPos + animIndex * 2);
	if (animOffset == 0 || animOffset >= size) {
		ImGui::TextColored(SOL_BASE01, "No animation data.");
		ImGui::EndChild();
		return;
	}

	if ((uint32)animOffset + 2 > size) {
		ImGui::TextColored(SOL_RED, "Invalid animation offset.");
		ImGui::EndChild();
		return;
	}

	const byte *animData = data + animOffset;
	uint16 limbMask = READ_LE_UINT16(animData);

	// Animation properties header
	char headerBuf[64];
	Common::sprintf_s(headerBuf, "Animation %d Properties", animIndex);
	ImGui::SeparatorText(headerBuf);

	// Limb selection
	ImGui::TextColored(SOL_BASE01, "Limbs:");
	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 0));

	for (int limb = 0; limb < 16; ++limb) {
		bool active = (limbMask & (1 << (15 - limb))) != 0;
		bool selected = (_selectedLimb == limb);

		ImGui::PushID(limb);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, selected ? 2.0f : 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, selected ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.39f, 0.39f, 0.39f, 1.0f));

		ImVec4 color = active ? ImVec4(0.18f, 0.545f, 0.341f, 1.0f) : ImVec4(0.235f, 0.235f, 0.235f, 1.0f);
		if (ImGui::ColorButton("##limb", color, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop)) {
			if (_selectedLimb == limb) {
				_selectedLimb = -1;
				_selectedCommand = -1;
			} else {
				_selectedLimb = limb;
				_selectedCommand = (_animCurPos[limb] != 0xFFFF) ? _animCurPos[limb] : -1;
			}
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopID();

		if (limb < 15)
			ImGui::SameLine();
	}

	ImGui::PopStyleVar(2);
	ImGui::Spacing();

	if (_selectedLimb >= 0) {
		renderLimbProperties(data, size, format, numColors, limbMask, animCmdsOffset);
	} else {
		ImGui::TextColored(SOL_BASE01, "Select a limb.");
	}

	ImGui::EndChild();
}

void Costume::renderAnimationPreview() {
	ImGui::BeginChild("AnimationPreview", ImVec2(0, 0), ImGuiChildFlags_Borders);

	ImGui::SeparatorText("Animation Preview");

	if (_selectedCostume < 0 || _selectedAnimGroup < 0) {
		ImGui::EndChild();
		return;
	}

	COST *cost = findCostume(_selectedCostume);
	if (!cost || cost->data.size() < 8) {
		ImGui::EndChild();
		return;
	}

	const byte *data = cost->data.data();
	uint32 size = cost->data.size();
	uint32 costumeSize = READ_LE_UINT32(data);
	byte numAnims = data[6];
	if (costumeSize == 0)
		numAnims++;
	byte format = data[7];
	int numColors = (format & 0x01) ? 32 : 16;
	int animIndex = _selectedAnimGroup * 4 + _selectedDirection;

	// Re-initialize if selection changed
	if (_lastCostume != _selectedCostume || _lastAnimGroup != _selectedAnimGroup || _lastDirection != (int)_selectedDirection) {
		_lastCostume = _selectedCostume;
		_lastAnimGroup = _selectedAnimGroup;
		_lastDirection = (int)_selectedDirection;
		initAnimState(data, size, format, numColors, animIndex);
	}
	if (animIndex >= numAnims) {
		ImGui::EndChild();
		return;
	}

	// Get animation dimensions
	LimbDimensions animDims = getAnimationDimensions(data, size, format, numColors, animIndex);
	if (!animDims.valid) {
		ImGui::EndChild();
		return;
	}

	int previewWidth = animDims.width();
	int previewHeight = animDims.height();

	if (previewWidth <= 0 || previewHeight <= 0) {
		ImGui::EndChild();
		return;
	}

	// Controls
	static int animScale = 4;
	static bool animTransparent = true;
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("Scale", &animScale, 1, 8, "%dx");
	ImGui::SameLine();
	ImGui::Checkbox("Transparent", &animTransparent);
	ImGui::SameLine();
	ImGui::Checkbox("Show Limbs", &_showAnimBounds);

	// Get palettes
	const Common::Array<Color> *roomColors = getRoomColors();
	Common::Array<byte> costumePalette = getCostumePalette(data, size);

	// Mirror if not facing West or if costume mirror flag set
	bool mirror = (_selectedDirection != DIR_WEST) || ((format & 0x80) != 0);
	int scale = animScale;

	ImGui::BeginChild("AnimPreviewScroll", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	ImVec2 bgP1 = canvasPos;
	ImVec2 bgP2(canvasPos.x + previewWidth * scale, canvasPos.y + previewHeight * scale);

	// Draw background
	if (!animTransparent)
		drawList->AddRectFilled(bgP1, bgP2, getBackgroundColor(costumePalette, roomColors));

	// Get animation data
	int animOffsetsPos = 8 + numColors + 2 + 32;
	uint16 animCmdsOffset = READ_LE_UINT16(data + 8 + numColors);
	uint16 animOffset = READ_LE_UINT16(data + animOffsetsPos + animIndex * 2);

	if (animOffset > 0 && (uint32)animOffset + 2 <= size) {
		const byte *animData = data + animOffset;
		uint16 limbMask = READ_LE_UINT16(animData);
		const byte *animCmds = data + animCmdsOffset;

		// Limb info for two-pass rendering
		struct LimbRenderInfo { int limb; uint16 startOffset; uint16 endOffset; PictureInfo pic; };
		Common::Array<LimbRenderInfo> limbsToRender;
		const byte *limbDataPtr = animData + 2;
		uint32 remainingSize = size - (animOffset + 2);

		// First pass: draw limb pictures
		for (int limb = 0; limb < 16 && remainingSize >= 2; ++limb) {
			bool isActive = (limbMask & (1 << (15 - limb))) != 0;
			if (!isActive)
				continue;

			uint16 startOffset = READ_LE_UINT16(limbDataPtr);
			limbDataPtr += 2;
			remainingSize -= 2;
			if (startOffset == 0xFFFF)
				continue;
			if (remainingSize < 1)
				break;

			byte extra = *limbDataPtr;
			limbDataPtr += 1;
			remainingSize -= 1;
			uint16 endOffset = startOffset + (extra & 0x7F);

			byte picIdx = getLimbPicture(limb, animCmds);
			if (picIdx == 0xFF || picIdx >= 0x71)
				continue;

			PictureInfo pic = getPictureInfo(data, size, format, limb, picIdx);
			if (pic.valid) {
				int picX = mirror ? (animDims.maxX - pic.relX - pic.width) * scale : (pic.relX - animDims.minX) * scale;
				int picY = (pic.relY - animDims.minY) * scale;
				ImVec2 picPos(canvasPos.x + picX, canvasPos.y + picY);
				decodePicture(drawList, picPos, scale, pic, format, costumePalette, roomColors, mirror, animTransparent);

				LimbRenderInfo info = { limb, startOffset, endOffset, pic };
				limbsToRender.push_back(info);
			}
		}

		// Second pass: draw bounding boxes
		if (_showAnimBounds) {
			for (uint i = 0; i < limbsToRender.size(); ++i) {
				const LimbRenderInfo &info = limbsToRender[i];

				LimbDimensions limbDims = getLimbMaxDimensions(data, size, format, info.limb, info.startOffset, info.endOffset, animCmdsOffset);
				if (!limbDims.valid)
					continue;

				int limbX = mirror ? (animDims.maxX - limbDims.maxX) * scale : (limbDims.minX - animDims.minX) * scale;
				int limbY = (limbDims.minY - animDims.minY) * scale;
				ImVec2 boxP1(canvasPos.x + limbX, canvasPos.y + limbY);
				ImVec2 boxP2(boxP1.x + limbDims.width() * scale, boxP1.y + limbDims.height() * scale);

				if (_selectedLimb == info.limb)
					drawList->AddRectFilled(boxP1, boxP2, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 0.75f)));
				drawList->AddRect(boxP1, boxP2, IM_COL32(255, 255, 0, 255), 0.0f, 0, 1.0f);
			}
		}
	}

	ImGui::Dummy(ImVec2((float)previewWidth * scale, (float)previewHeight * scale));

	ImGui::EndChild();

	ImGui::EndChild();
}

void Costume::render(ImGuiID dockSpaceId, bool *open) {
	ImGui::SetNextWindowDockID(dockSpaceId, ImGuiCond_FirstUseEver);
	ImGui::Begin(ICON_COSTUME " Costume", open);

	renderCostumeList();
	ImGui::SameLine();
	renderCostumeProperties();
	ImGui::SameLine();
	renderAnimationPreview();

	ImGui::End();
}

} // End of namespace Editor

} // End of namespace Scumm
