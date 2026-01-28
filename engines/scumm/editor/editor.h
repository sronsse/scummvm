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

#ifndef SCUMM_EDITOR_H
#define SCUMM_EDITOR_H

#include "common/path.h"
#include "common/str.h"

#include "scumm/editor/explorer.h"
#include "scumm/editor/game.h"
#include "scumm/editor/room.h"
#include "scumm/editor/screen.h"

namespace Scumm {

class ScummEngine;

class ScummEditor {
private:
	ScummEngine *_engine;

	Common::String _gameName;
	Common::Path _gamePath;
	Common::Path _indexPath;
	Common::Path _mainPath;
	byte _encByte;

	Editor::RNAM _rnam;
	Editor::MAXS _maxs;
	Editor::DROO _droo;
	Editor::DSCR _dscr;
	Editor::DSOU _dsou;
	Editor::DCOS _dcos;
	Editor::DCHR _dchr;
	Editor::DOBJ _dobj;
	Editor::AARY _aary;
	Editor::LECF _lecf;

	Common::Array<byte> _snapshotIndex;
	Common::Array<byte> _snapshotMain;
	Common::Array<byte> _serializedIndex;
	Common::Array<byte> _serializedMain;

	Editor::Screen _screen;
	Editor::Explorer _explorer;
	Editor::Game _game;
	Editor::Room _room;

	void load();
	void save();
	void serialize();

	void readDirectories();
	void writeDirectories();

public:
	ScummEditor(ScummEngine *engine);

	void render();
};

} // End of namespace Scumm

#endif
