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

Explorer::Explorer(const Common::Array<byte> &indexData, const Common::Array<byte> &mainData)
	: _indexData(indexData),
	  _mainData(mainData),
	  _selectedIndexBlock(-1),
	  _selectedMainBlock(-1) {
}

void Explorer::buildTree(Common::Array<Block> &blocks, const byte *data, uint32 dataSize, uint32 baseOffset, int parentIndex) {
	uint32 offset = 0;

	while (offset + 8 <= dataSize) {
		uint32 tag = READ_BE_UINT32(data + offset);
		uint32 size = READ_BE_UINT32(data + offset + 4);

		// Validate tag bytes are printable ASCII
		bool validTag = true;
		for (int i = 0; i < 4; ++i) {
			byte b = (tag >> (24 - i * 8)) & 0xFF;
			if (b < 0x20 || b > 0x7E) {
				validTag = false;
				break;
			}
		}
		if (!validTag || size < 8 || offset + size > dataSize)
			break;

		// Create block entry
		Block block;
		block.tag = tag;
		block.offset = baseOffset + offset;
		block.size = size;
		block.parent = parentIndex;

		int blockIndex = blocks.size();
		blocks.push_back(block);

		if (parentIndex >= 0)
			blocks[parentIndex].children.push_back(blockIndex);

		// Check for container blocks
		bool isContainer = false;
		switch (tag) {
		case MKTAG('L','E','C','F'):
		case MKTAG('L','F','L','F'):
		case MKTAG('R','O','O','M'):
		case MKTAG('R','M','I','M'):
		case MKTAG('O','B','I','M'):
		case MKTAG('O','B','C','D'):
		case MKTAG('P','A','L','S'):
		case MKTAG('W','R','A','P'):
		case MKTAG('S','O','U','N'):
		case MKTAG('S','O','U',' '):
			isContainer = true;
			break;
		default:
			break;
		}

		// Handle IMxx blocks
		byte b0 = (tag >> 24) & 0xFF;
		byte b1 = (tag >> 16) & 0xFF;
		if (b0 == 'I' && b1 == 'M')
			isContainer = true;

		// Recurse into container blocks
		if (isContainer)
			buildTree(blocks, data + offset + 8, size - 8, baseOffset + offset + 8, blockIndex);

		offset += size;
	}
}

void Explorer::render(ImGuiID dockSpaceId, bool *open) {
	ImGui::SetNextWindowDockID(dockSpaceId, ImGuiCond_FirstUseEver);
	ImGui::Begin(ICON_RESOURCE " Explorer", open);

	if (ImGui::BeginTabBar("ExplorerTabs")) {
		if (ImGui::BeginTabItem("Index")) {
			ImGui::PushID(0);
			renderTab(_indexData, _selectedIndexBlock);
			ImGui::PopID();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Main")) {
			ImGui::PushID(1);
			renderTab(_mainData, _selectedMainBlock);
			ImGui::PopID();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Explorer::renderTab(const Common::Array<byte> &data, int &selectedBlock) {
	// Build block tree
	Common::Array<Block> blocks;
	buildTree(blocks, data.data(), data.size(), 0, -1);

	if (selectedBlock >= (int)blocks.size())
		selectedBlock = -1;

	// Render tree and block view
	renderTree(blocks, selectedBlock);
	ImGui::SameLine();
	renderBlock(blocks, selectedBlock, data);
}

void Explorer::renderTree(Common::Array<Block> &blocks, int &selectedBlock) {
	ImGui::BeginChild("BlockTree", ImVec2(350, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

	for (uint i = 0; i < blocks.size(); ++i) {
		if (blocks[i].parent == -1)
			renderTreeNode(blocks, i, selectedBlock);
	}

	ImGui::EndChild();
}

void Explorer::renderTreeNode(Common::Array<Block> &blocks, int index, int &selectedBlock) {
	const Block &block = blocks[index];

	// Setup tree node flags
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (block.children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;
	if (selectedBlock == index)
		flags |= ImGuiTreeNodeFlags_Selected;

	// Create label
	char label[64];
	Common::sprintf_s(label, "%s [0x%08X, %u bytes]", tag2str(block.tag), block.offset, block.size);

	bool nodeOpen = ImGui::TreeNodeEx((void *)(intptr_t)index, flags, "%s", label);

	if (ImGui::IsItemClicked())
		selectedBlock = index;

	// Render children
	if (nodeOpen) {
		for (uint i = 0; i < block.children.size(); ++i)
			renderTreeNode(blocks, block.children[i], selectedBlock);
		ImGui::TreePop();
	}
}

void Explorer::renderBlock(const Common::Array<Block> &blocks, int selectedBlock, const Common::Array<byte> &data) {
	ImGui::BeginChild("BlockHex", ImVec2(0, 0), ImGuiChildFlags_Borders);

	if (selectedBlock >= 0 && selectedBlock < (int)blocks.size()) {
		const Block &block = blocks[selectedBlock];

		// Header
		ImGui::SeparatorText(tag2str(block.tag));
		ImGui::TextColored(SOL_BASE0, "Offset: 0x%08X", block.offset);
		ImGui::TextColored(SOL_BASE0, "Size: %u bytes", block.size);

		// Data range
		uint32 dataOffset = block.offset;
		uint32 dataSize = block.size;
		if (dataOffset + dataSize > data.size())
			dataSize = data.size() - dataOffset;

		const byte *blockData = data.data() + dataOffset;
		uint32 totalLines = (dataSize + 15) / 16;

		// Hex view
		ImGui::BeginChild("HexScrollRegion");

		ImGuiListClipper clipper;
		clipper.Begin(totalLines);
		while (clipper.Step()) {
			for (int line = clipper.DisplayStart; line < clipper.DisplayEnd; ++line) {
				uint32 lineOffset = line * 16;
				uint32 lineBytes = (lineOffset + 16 <= dataSize) ? 16 : (dataSize - lineOffset);

				// Offset column
				ImGui::TextColored(SOL_BASE01, "%08X:", dataOffset + lineOffset);
				ImGui::SameLine();

				// Hex bytes
				char hexBuf[128];
				int hexPos = 0;
				for (uint32 i = 0; i < 16; ++i) {
					if (i == 8)
						hexBuf[hexPos++] = ' ';
					if (i < lineBytes) {
						Common::sprintf_s(hexBuf + hexPos, sizeof(hexBuf) - hexPos, "%02X ", blockData[lineOffset + i]);
						hexPos += 3;
					} else {
						hexBuf[hexPos++] = ' ';
						hexBuf[hexPos++] = ' ';
						hexBuf[hexPos++] = ' ';
					}
				}
				hexBuf[hexPos] = '\0';
				ImGui::TextColored(SOL_GREEN, "%s", hexBuf);
				ImGui::SameLine();

				// ASCII column
				char asciiBuf[20];
				for (uint32 i = 0; i < 16; ++i) {
					if (i < lineBytes) {
						byte b = blockData[lineOffset + i];
						asciiBuf[i] = (b >= 0x20 && b <= 0x7E) ? (char)b : '.';
					} else {
						asciiBuf[i] = ' ';
					}
				}
				asciiBuf[16] = '\0';
				ImGui::TextColored(SOL_YELLOW, "%s", asciiBuf);
			}
		}

		ImGui::EndChild();
	} else {
		ImGui::TextColored(SOL_BASE01, "Select a block.");
	}

	ImGui::EndChild();
}

} // End of namespace Editor

} // End of namespace Scumm
