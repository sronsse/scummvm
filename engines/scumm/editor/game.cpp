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

#include "scumm/editor/editor.h"
#include "scumm/editor/style.h"

namespace Scumm {

namespace Editor {

Game::Game(const Common::String &gameName, const byte &encByte, RNAM &rnam, MAXS &maxs, DROO &droo, DSCR &dscr, DSOU &dsou, DCOS &dcos, DCHR &dchr, DOBJ &dobj, AARY &aary)
	: _gameName(gameName),
	  _encByte(encByte),
	  _rnam(rnam),
	  _maxs(maxs),
	  _droo(droo),
	  _dscr(dscr),
	  _dsou(dsou),
	  _dcos(dcos),
	  _dchr(dchr),
	  _dobj(dobj),
	  _aary(aary) {
}

void Game::renderInfo() {
	ImGui::TextColored(SOL_BLUE, "Name");
	ImGui::TextColored(SOL_BASE0, "%s", _gameName.c_str());

	ImGui::TextColored(SOL_BLUE, "Encryption");
	ImGui::TextColored(SOL_BASE0, "0x%02X", _encByte);
}

void Game::renderRoomNames() {
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("RoomNames", 2, flags)) {
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Name");
		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;
		clipper.Begin(_rnam.locations.size());
		while (clipper.Step()) {
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
				ImGui::TableNextRow();
				ImGui::PushID(i);

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", _rnam.locations[i]);

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%s", _rnam.names[i].c_str());

				ImGui::PopID();
			}
		}

		ImGui::EndTable();
	}
}

void Game::renderMaximums() {
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("Maximums", 2, flags)) {
		ImGui::TableSetupColumn("Property");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		auto row = [](const char *label, uint16 value) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BLUE, "%s", label);
			ImGui::TableNextColumn();
			ImGui::TextColored(SOL_BASE0, "%d", value);
		};

		row("Variables", _maxs.numVariables);
		row("Bit Variables", _maxs.numBitVariables);
		row("Local Objects", _maxs.numLocalObjects);
		row("Arrays", _maxs.numArray);
		row("Verbs", _maxs.numVerbs);
		row("FL Objects", _maxs.numFlObject);
		row("Inventory", _maxs.numInventory);
		row("Rooms", _maxs.numRooms);
		row("Scripts", _maxs.numScripts);
		row("Sounds", _maxs.numSounds);
		row("Charsets", _maxs.numCharsets);
		row("Costumes", _maxs.numCostumes);
		row("Global Objects", _maxs.numGlobalObjects);

		ImGui::EndTable();
	}
}

void Game::renderDirectories() {
	if (ImGui::CollapsingHeader("Rooms", ImGuiTreeNodeFlags_DefaultOpen))
		renderDirectory("Rooms", _droo.locations, _droo.offsets);

	if (ImGui::CollapsingHeader("Scripts", ImGuiTreeNodeFlags_DefaultOpen))
		renderDirectory("Scripts", _dscr.locations, _dscr.offsets);

	if (ImGui::CollapsingHeader("Sounds", ImGuiTreeNodeFlags_DefaultOpen))
		renderDirectory("Sounds", _dsou.locations, _dsou.offsets);

	if (ImGui::CollapsingHeader("Costumes", ImGuiTreeNodeFlags_DefaultOpen))
		renderDirectory("Costumes", _dcos.locations, _dcos.offsets);

	if (ImGui::CollapsingHeader("Charsets", ImGuiTreeNodeFlags_DefaultOpen))
		renderDirectory("Charsets", _dchr.locations, _dchr.offsets);
}

void Game::renderDirectory(const char *label, const Common::Array<byte> &locations, const Common::Array<uint32> &offsets) {
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;
	float rowHeight = ImGui::GetTextLineHeightWithSpacing();
	float tableHeight = MIN(rowHeight * (locations.size() + 1), 300.0f);

	if (ImGui::BeginTable(label, 3, flags, ImVec2(0.0f, tableHeight))) {
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Room");
		ImGui::TableSetupColumn("Offset");
		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;
		clipper.Begin(locations.size());
		while (clipper.Step()) {
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", i);
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", locations[i]);
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_GREEN, "0x%08X", offsets[i]);
			}
		}

		ImGui::EndTable();
	}
}

void Game::renderObjects() {
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("Objects", 4, flags)) {
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Owner");
		ImGui::TableSetupColumn("State");
		ImGui::TableSetupColumn("Class");
		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;
		clipper.Begin(_dobj.ownerTable.size());
		while (clipper.Step()) {
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
				ImGui::TableNextRow();
				ImGui::PushID(i);

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", i);

				// Owner field
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", _dobj.ownerTable[i] & 0x0F);

				// State field
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", (_dobj.ownerTable[i] >> 4) & 0x0F);

				// Class field
				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%08X", _dobj.classData[i]);

				ImGui::PopID();
			}
		}

		ImGui::EndTable();
	}
}

void Game::renderArrays() {
	const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("Arrays", 4, flags)) {
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("Variable");
		ImGui::TableSetupColumn("Dim A");
		ImGui::TableSetupColumn("Dim B");
		ImGui::TableSetupColumn("Type");
		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;
		clipper.Begin(_aary.arrays.size());
		while (clipper.Step()) {
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
				ImGui::TableNextRow();
				ImGui::PushID(i);

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", _aary.arrays[i].varNumber);

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", _aary.arrays[i].dimA);

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", _aary.arrays[i].dimB);

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE0, "%d", _aary.arrays[i].type);

				ImGui::PopID();
			}
		}

		ImGui::EndTable();
	}
}

void Game::render(ImGuiID dockSpaceId, bool *open) {
	ImGui::SetNextWindowDockID(dockSpaceId, ImGuiCond_FirstUseEver);
	ImGui::Begin(ICON_GAME " Game", open);

	renderInfo();
	ImGui::Separator();

	if (ImGui::BeginTabBar("GameTabs")) {
		if (ImGui::BeginTabItem("Room Names")) {
			renderRoomNames();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Maximums")) {
			renderMaximums();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Directories")) {
			renderDirectories();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Objects")) {
			renderObjects();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Arrays")) {
			renderArrays();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

} // End of namespace Editor

} // End of namespace Scumm
