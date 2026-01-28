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

#ifndef SCUMM_EDITOR_ROOM_H
#define SCUMM_EDITOR_ROOM_H

#include "backends/imgui/imgui.h"

#include "common/array.h"

#include "scumm/editor/resource.h"

namespace Scumm {

namespace Editor {

class Room {
private:
	RNAM &_rnam;
	DROO &_droo;
	LECF &_lecf;
	int _selectedRoom;

	int _selectedObject;
	int _selectedImage;
	bool _showObjects;
	Common::Array<bool> _objectVisibility;

	int _selectedBox;
	bool _showBoxes;

	int _selectedMask;
	bool _showMasks;
	Common::Array<bool> _maskVisibility;

	int _selectedCycle;

	void *_roomTexture;
	void *_tooltipTexture;
	int _tooltipRoomId;

	LFLF *findRoom(int roomId);
	void *createRoomTexture(LFLF *lflf);
	void *createTooltipTexture(LFLF *lflf);
	void renderRoomTooltip(int roomId);
	void drawObjectImage(ImDrawList *drawList, ImVec2 pos, int scale, LFLF *lflf, uint16 objectId, int imageIndex, bool transparent);
	void renderObjectTooltip(LFLF *lflf, uint16 objectId, int imageIndex = 0);
	void renderObjectImagesTab(LFLF *lflf, uint16 objectId);
	void renderObjectImagePreview(LFLF *lflf, uint16 objectId, int imageIndex);
	void drawMask(ImDrawList *drawList, ImVec2 pos, float scale, LFLF *lflf, int maskIndex, ImU32 color);
	void renderMaskTooltip(LFLF *lflf, int maskIndex);

	void renderRoomList();
	void renderRoomProperties();
	void renderPalette(LFLF *lflf);
	void renderCyclesList(LFLF *lflf);
	void renderMasksList(LFLF *lflf);
	void renderBoxesList(LFLF *lflf);
	void renderBoxProperties(LFLF *lflf, int boxIndex, Box &box);
	void renderObjectsList(LFLF *lflf);
	void renderObjectProperties(LFLF *lflf, OBCD &obcd);
	void renderObjectCodeProperties(LFLF *lflf, OBCD &obcd);
	void renderRoomImage();

public:
	Room(RNAM &rnam, DROO &droo, LECF &lecf);
	~Room();

	void render(ImGuiID dockSpaceId, bool *open);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
