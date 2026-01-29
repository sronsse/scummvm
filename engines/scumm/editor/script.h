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

#ifndef SCUMM_EDITOR_SCRIPT_H
#define SCUMM_EDITOR_SCRIPT_H

#include "backends/imgui/imgui.h"

#include "common/str.h"
#include "scumm/editor/resource.h"
#include "scumm/editor/compiler/compiler.h"

namespace Scumm {

namespace Editor {

enum ScriptType {
	SCRIPT_NONE,
	SCRIPT_GLOBAL,   // SCRP
	SCRIPT_ENTRANCE, // ENCD
	SCRIPT_EXIT,     // EXCD
	SCRIPT_LOCAL     // LSCR
};

class Script {
private:
	DSCR &_dscr;
	LECF &_lecf;

	ScriptType _selectedType;
	int _selectedScript;  // Global script ID, or local script index (1-based)
	int _selectedRoom;    // Room index in lflfs array

	Common::String _sourceText;
	Common::String _compileLog;
	ScriptType _sourceType;
	int _sourceScriptId;
	int _sourceRoom;
	bool _sourceEditing;
	bool _compileSuccess;
	Common::Array<byte> _compiledBytecode;

	SCRP *findGlobalScript(int scriptId);
	LFLF *findLFLF(int roomIndex);
	Common::Array<byte> *getSelectedScriptData();
	void renderScriptList();
	void renderDisassembly();
	void renderSource();
	Common::String generateSource();

public:
	Script(DSCR &dscr, LECF &lecf);

	void render(ImGuiID dockSpaceId, bool *open);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
