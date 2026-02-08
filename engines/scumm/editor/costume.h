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

#ifndef SCUMM_EDITOR_COSTUME_H
#define SCUMM_EDITOR_COSTUME_H

#include "backends/imgui/imgui.h"

#include "scumm/editor/resource.h"

namespace Scumm {

namespace Editor {

// Picture information structure
struct PictureInfo {
	uint16 width;
	uint16 height;
	int16 relX;
	int16 relY;
	const byte *rleData;
	uint32 rleSize;
	bool valid;
	bool redirected;
	byte redirLimb;
	byte redirPict;

	PictureInfo() : width(0), height(0), relX(0), relY(0), rleData(nullptr), rleSize(0),
	                valid(false), redirected(false), redirLimb(0xFF), redirPict(0xFF) {}
};

// Limb dimensions structure
struct LimbDimensions {
	int minX;
	int minY;
	int maxX;
	int maxY;
	bool valid;

	LimbDimensions() : minX(0), minY(0), maxX(0), maxY(0), valid(false) {}

	int width() const { return valid ? (maxX - minX) : 0; }
	int height() const { return valid ? (maxY - minY) : 0; }
};

class Costume {
public:
	// Actor/animation facing directions
	enum Direction {
		DIR_WEST = 0,
		DIR_EAST = 1,
		DIR_SOUTH = 2,
		DIR_NORTH = 3
	};

private:
	DCOS &_dcos;
	LECF &_lecf;
	int _selectedCostume;
	int _selectedPaletteRoom;  // -1 means use costume's room
	int _selectedAnimGroup;    // Selected animation group (0, 1, 2, ...)
	Direction _selectedDirection;
	int _selectedLimb;         // Selected limb in animation (0-15, -1 if none)
	int _selectedCommand;      // Selected command index (-1 if none)
	bool _showBoundingBox;     // Toggle for picture bounding box visibility
	bool _showAnimBounds;      // Toggle for animation limb bounds visibility
	// Animation playback state
	uint16 _animCurPos[16];    // Current position in anim commands for each limb
	uint16 _animStart[16];     // Start position for each limb
	uint16 _animEnd[16];       // End position for each limb
	bool _animLooping[16];     // Whether each limb loops
	int _animCounter;          // Global animation step counter
	int _animMaxCounter;       // Maximum counter value (LCM of limb lengths or max length)
	bool _animPlaying;         // Whether animation is currently playing
	byte _animSpeed;           // Frame skip count (0 = every frame, mimics engine's _animSpeed)
	byte _animProgress;        // Current frame counter (mimics engine's _animProgress)
	float _animFrameAccum;     // Accumulator for sub-frame timing (to simulate 60 FPS)
	int _lastAnimGroup;        // Last selected animation group (to detect changes)
	int _lastDirection;        // Last selected direction (to detect changes)
	int _lastCostume;          // Last selected costume (to detect changes)

	// Costume lookup helpers
	COST *findCostume(int costumeId);
	LFLF *findLFLF(int costumeId);
	LFLF *findLFLFByRoom(int roomNum);
	static Common::Array<byte> getCostumePalette(const byte *data, uint32 size);
	const Common::Array<Color> *getRoomColors();

	// Picture data helpers
	PictureInfo getPictureInfo(const byte *data, uint32 size, byte format, int limbIdx, int picIdx);
	LimbDimensions getLimbMaxDimensions(const byte *data, uint32 size, byte format, int limbIdx, uint16 startOffset, uint16 endOffset, uint16 animCmdsOffset);
	LimbDimensions getAnimationDimensions(const byte *data, uint32 size, byte format, int numColors, int animIndex);

	// Rendering helpers
	void decodePicture(ImDrawList *drawList, ImVec2 pos, int scale, const PictureInfo &pic, byte format, const Common::Array<byte> &costumePalette, const Common::Array<Color> *roomColors, bool mirror = false, bool transparent = false);
	void renderPicturePreview(const PictureInfo &pic, byte format, const Common::Array<byte> &costumePalette, const LimbDimensions &limbDims, const Common::Array<Color> *roomColors);

	// Animation playback helpers
	void initAnimState(const byte *data, uint32 size, byte format, int numColors, int animIndex);
	void stepAnimForward(const byte *data, uint32 size, uint16 animCmdsOffset);
	void stepAnimBackward(const byte *data, uint32 size, uint16 animCmdsOffset);
	void seekAnimTo(const byte *data, uint32 size, byte format, int numColors, int animIndex, int targetCounter);
	byte getLimbPicture(int limb, const byte *animCmds) const;

	// Tooltip helpers
	bool renderAnimFirstFrame(const byte *data, uint32 size, byte format, int numColors, int animIndex, const Common::Array<byte> &costumePalette, const Common::Array<Color> *roomColors);
	void renderCostumeTooltip(int costumeId);
	void renderAnimTooltip(int animGroup);

	// UI rendering
	void renderCostumeList();
	void renderCostumeProperties();
	void renderPaletteTab();
	void renderAnimationsTab();
	void renderLimbProperties(const byte *data, uint32 size, byte format, int numColors, uint16 limbMask, uint16 animCmdsOffset);
	void renderCommandList(const byte *data, uint32 size, byte format, int numColors, uint16 startOffset, uint16 endOffset, uint16 animCmdsOffset, const LimbDimensions &limbDims);
	void renderAnimationPreview();

public:
	Costume(DCOS &dcos, LECF &lecf);

	void render(ImGuiID dockSpaceId, bool *open);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
