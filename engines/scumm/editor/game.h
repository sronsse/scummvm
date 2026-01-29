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

#ifndef SCUMM_EDITOR_GAME_H
#define SCUMM_EDITOR_GAME_H

#include "backends/imgui/imgui.h"

#include "scumm/editor/resource.h"

namespace Scumm {

namespace Editor {

class Game {
private:
	const Common::String &_gameName;
	const byte &_encByte;

	RNAM &_rnam;
	MAXS &_maxs;
	DROO &_droo;
	DSCR &_dscr;
	DSOU &_dsou;
	DCOS &_dcos;
	DCHR &_dchr;
	DOBJ &_dobj;
	AARY &_aary;

	void renderInfo();
	void renderRoomNames();
	void renderMaximums();
	void renderDirectories();
	void renderDirectory(const char *label, const Common::Array<byte> &locations, const Common::Array<uint32> &offsets);
	void renderObjects();
	void renderArrays();

public:
	Game(const Common::String &gameName, const byte &encByte, RNAM &rnam, MAXS &maxs, DROO &droo, DSCR &dscr, DSOU &dsou, DCOS &dcos, DCHR &dchr, DOBJ &dobj, AARY &aary);

	void render(ImGuiID dockSpaceId, bool *open);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
