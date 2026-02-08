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

#include "backends/imgui/imgui.h"

#include "common/config-manager.h"
#include "common/system.h"

#include "scumm/scumm.h"

#include "scumm/editor/editor.h"
#include "scumm/editor/style.h"

namespace Scumm {

ScummEditor::ScummEditor(ScummEngine *engine)
	: _engine(engine),
	  _gameName(ConfMan.get("gameid")),
	  _gamePath(ConfMan.getPath("path")),
	  _indexPath(_gamePath.join(engine->generateFilename(0).toString())),
	  _mainPath(_gamePath.join(engine->generateFilename(1).toString())),
	  _encByte(engine->getEncByte(0)),
	  _screen(engine),
	  _explorer(_serializedIndex, _serializedMain),
	  _game(_gameName, _encByte, _rnam, _maxs, _droo, _dscr, _dsou, _dcos, _dchr, _dobj, _aary),
	  _room(_rnam, _droo, _lecf),
	  _charset(_dchr, _lecf),
	  _costume(_dcos, _lecf),
	  _script(_dscr, _lecf) {
	// Verify version
	if (_engine->_game.version != 6)
		error("Editor only supports SCUMM v6");

	load();
}

void ScummEditor::load() {
	// Reset resources
	_rnam = Editor::RNAM();
	_maxs = Editor::MAXS();
	_droo = Editor::DROO();
	_dscr = Editor::DSCR();
	_dsou = Editor::DSOU();
	_dcos = Editor::DCOS();
	_dchr = Editor::DCHR();
	_dobj = Editor::DOBJ();
	_aary = Editor::AARY();
	_lecf = Editor::LECF();

	// Read index file
	Editor::Resource res(_encByte);
	if (!res.openRead(_indexPath)) {
		warning("ScummEditor::build: Could not open %s", _indexPath.toString().c_str());
		return;
	}

	res.readHeader("RNAM");
	res.readRNAM(&_rnam);
	res.readHeader("MAXS");
	res.readMAXS(&_maxs);
	res.readHeader("DROO");
	res.readDROO(&_droo);
	res.readHeader("DSCR");
	res.readDSCR(&_dscr);
	res.readHeader("DSOU");
	res.readDSOU(&_dsou);
	res.readHeader("DCOS");
	res.readDCOS(&_dcos);
	res.readHeader("DCHR");
	res.readDCHR(&_dchr);
	res.readHeader("DOBJ");
	res.readDOBJ(&_dobj);
	res.readHeader("AARY");
	res.readAARY(&_aary);
	_snapshotIndex = res.data();
	res.close();

	// Read main file
	if (!res.openRead(_mainPath)) {
		warning("ScummEditor::build: Could not open %s", _mainPath.toString().c_str());
		return;
	}

	res.readHeader("LECF");
	res.readLECF(&_lecf);
	_snapshotMain = res.data();
	res.close();

	// Assign IDs to resources
	readDirectories();
}

void ScummEditor::readDirectories() {
	for (uint i = 0; i < _lecf.loff.locations.size(); ++i) {
		if (_lecf.loff.locations[i] == 0)
			continue;

		// Assign ROOM ID
		for (uint j = 0; j < _droo.locations.size(); ++j) {
			if (_droo.locations[j] == _lecf.loff.locations[i]) {
				_lecf.lflfs[i].room.id = j;
				break;
			}
		}

		// Set initial directory offset
		uint32 dirOffset = Editor::Resource::getROOMSize(&_lecf.lflfs[i].room);

		// Assign SCRP IDs
		for (uint k = 0; k < _lecf.lflfs[i].scrps.size(); ++k) {
			_lecf.lflfs[i].scrps[k].id = -1;
			for (uint j = 0; j < _dscr.locations.size(); ++j) {
				if (_dscr.locations[j] == _lecf.loff.locations[i] && _dscr.offsets[j] == dirOffset) {
					_lecf.lflfs[i].scrps[k].id = j;
					break;
				}
			}
			dirOffset += Editor::Resource::getSCRPSize(&_lecf.lflfs[i].scrps[k]);
		}

		// Assign SOUN IDs
		for (uint k = 0; k < _lecf.lflfs[i].souns.size(); ++k) {
			_lecf.lflfs[i].souns[k].id = -1;
			for (uint j = 0; j < _dsou.locations.size(); ++j) {
				if (_dsou.locations[j] == _lecf.loff.locations[i] && _dsou.offsets[j] == dirOffset) {
					_lecf.lflfs[i].souns[k].id = j;
					break;
				}
			}
			dirOffset += Editor::Resource::getSOUNSize(&_lecf.lflfs[i].souns[k]);
		}

		// Assign COST IDs
		for (uint k = 0; k < _lecf.lflfs[i].costs.size(); ++k) {
			_lecf.lflfs[i].costs[k].id = -1;
			for (uint j = 0; j < _dcos.locations.size(); ++j) {
				if (_dcos.locations[j] == _lecf.loff.locations[i] && _dcos.offsets[j] == dirOffset) {
					_lecf.lflfs[i].costs[k].id = j;
					break;
				}
			}
			dirOffset += Editor::Resource::getCOSTSize(&_lecf.lflfs[i].costs[k]);
		}

		// Assign CHAR IDs
		for (uint k = 0; k < _lecf.lflfs[i].chars.size(); ++k) {
			_lecf.lflfs[i].chars[k].id = -1;
			for (uint j = 0; j < _dchr.locations.size(); ++j) {
				if (_dchr.locations[j] == _lecf.loff.locations[i] && _dchr.offsets[j] == dirOffset) {
					_lecf.lflfs[i].chars[k].id = j;
					break;
				}
			}
			dirOffset += Editor::Resource::getCHARSize(&_lecf.lflfs[i].chars[k]);
		}
	}
}

void ScummEditor::writeDirectories() {
	// Set initial room offset
	uint32 roomOffset = Editor::Resource::getLOFFSize(&_lecf.loff) + 16;

	for (uint i = 0; i < _lecf.loff.locations.size(); ++i) {
		if (_lecf.loff.locations[i] == 0)
			continue;

		// Update ROOM offset
		_lecf.loff.offsets[i] = roomOffset;
		roomOffset += Editor::Resource::getLFLFSize(&_lecf.lflfs[i]);

		// Set initial directory offset
		uint32 dirOffset = Editor::Resource::getROOMSize(&_lecf.lflfs[i].room);

		// Update SCRP offsets
		for (uint j = 0; j < _lecf.lflfs[i].scrps.size(); ++j) {
			if (_lecf.lflfs[i].scrps[j].id != -1)
				_dscr.offsets[_lecf.lflfs[i].scrps[j].id] = dirOffset;
			dirOffset += Editor::Resource::getSCRPSize(&_lecf.lflfs[i].scrps[j]);
		}

		// Update SOUN offsets
		for (uint j = 0; j < _lecf.lflfs[i].souns.size(); ++j) {
			if (_lecf.lflfs[i].souns[j].id != -1)
				_dsou.offsets[_lecf.lflfs[i].souns[j].id] = dirOffset;
			dirOffset += Editor::Resource::getSOUNSize(&_lecf.lflfs[i].souns[j]);
		}

		// Update COST offsets
		for (uint j = 0; j < _lecf.lflfs[i].costs.size(); ++j) {
			if (_lecf.lflfs[i].costs[j].id != -1)
				_dcos.offsets[_lecf.lflfs[i].costs[j].id] = dirOffset;
			dirOffset += Editor::Resource::getCOSTSize(&_lecf.lflfs[i].costs[j]);
		}

		// Update CHAR offsets
		for (uint j = 0; j < _lecf.lflfs[i].chars.size(); ++j) {
			if (_lecf.lflfs[i].chars[j].id != -1)
				_dchr.offsets[_lecf.lflfs[i].chars[j].id] = dirOffset;
			dirOffset += Editor::Resource::getCHARSize(&_lecf.lflfs[i].chars[j]);
		}
	}
}

void ScummEditor::serialize() {
	// Update directory offsets
	writeDirectories();

	// Write index file
	Editor::Resource res(_encByte);
	res.openMem();
	res.writeRNAM(&_rnam);
	res.writeMAXS(&_maxs);
	res.writeDROO(&_droo);
	res.writeDSCR(&_dscr);
	res.writeDSOU(&_dsou);
	res.writeDCOS(&_dcos);
	res.writeDCHR(&_dchr);
	res.writeDOBJ(&_dobj);
	res.writeAARY(&_aary);
	_serializedIndex = res.data();
	res.close();

	// Write main file
	res.openMem();
	res.writeLECF(&_lecf);
	_serializedMain = res.data();
	res.close();
}

void ScummEditor::save() {
	// Close resources
	_engine->closeRoom();

	// Serialize
	serialize();

	// Write index file
	Editor::File indexFile(_encByte);
	indexFile.openWrite(_indexPath);
	indexFile.write(_serializedIndex.data(), _serializedIndex.size());
	indexFile.close();

	// Write main file
	Editor::File mainFile(_encByte);
	mainFile.openWrite(_mainPath);
	mainFile.write(_serializedMain.data(), _serializedMain.size());
	mainFile.close();

	// Restart engine
	_engine->restart();

	// Save snapshot
	_snapshotIndex = _serializedIndex;
	_snapshotMain = _serializedMain;
}

void ScummEditor::render() {
	// Serialize resources
	serialize();

	// Keyboard shortcuts
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
		save();

	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Q)) {
		g_engine->quitGame();
		return;
	}

	// Visibility toggles
	static bool showScreen = true;
	static bool showExplorer = true;
	static bool showGame = true;
	static bool showRoom = true;
	static bool showCharset = true;
	static bool showCostume = true;
	static bool showScript = true;

	// Check for changes
	bool dirty = _serializedIndex != _snapshotIndex || _serializedMain != _snapshotMain;

	// Main window
	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	Common::String title = Common::String(ICON_EDITOR) + " " + (dirty ? "*" : "") + _gameName + " - ScummVM SCUMM Editor###Editor";
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive));
	ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	// Menu bar
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save", "Ctrl+S"))
				save();
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "Ctrl+Q"))
				g_engine->quitGame();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem(ICON_SCREEN " Screen", nullptr, &showScreen);
			ImGui::MenuItem(ICON_RESOURCE " Explorer", nullptr, &showExplorer);
			ImGui::Separator();
			ImGui::MenuItem(ICON_CHARSET " Charset", nullptr, &showCharset);
			ImGui::MenuItem(ICON_COSTUME " Costume", nullptr, &showCostume);
			ImGui::MenuItem(ICON_GAME " Game", nullptr, &showGame);
			ImGui::MenuItem(ICON_ROOM " Room", nullptr, &showRoom);
			ImGui::MenuItem(ICON_SCRIPT " Script", nullptr, &showScript);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Dock space
	ImGuiID dockSpaceId = ImGui::GetID("EditorDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0, 0), ImGuiDockNodeFlags_None);

	ImGui::End();
	ImGui::PopStyleColor();

	// Render windows
	if (showScreen)
		_screen.render(dockSpaceId, &showScreen);
	if (showExplorer)
		_explorer.render(dockSpaceId, &showExplorer);
	if (showGame)
		_game.render(dockSpaceId, &showGame);
	if (showRoom)
		_room.render(dockSpaceId, &showRoom);
	if (showCharset)
		_charset.render(dockSpaceId, &showCharset);
	if (showCostume)
		_costume.render(dockSpaceId, &showCostume);
	if (showScript)
		_script.render(dockSpaceId, &showScript);
}

} // End of namespace Scumm
