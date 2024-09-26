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

#ifndef SCUMM_EDITOR_EXPLORER_H
#define SCUMM_EDITOR_EXPLORER_H

#include "backends/imgui/imgui.h"

#include "common/array.h"
#include "common/str.h"

namespace Scumm {

namespace Editor {

class Explorer {
private:
	struct Block {
		uint32 tag;
		uint32 offset;
		uint32 size;
		int parent;
		Common::Array<int> children;
	};

	const Common::Array<byte> &_indexData;
	const Common::Array<byte> &_mainData;
	int _selectedIndexBlock;
	int _selectedMainBlock;

	void buildTree(Common::Array<Block> &blocks, const byte *data, uint32 dataSize, uint32 baseOffset, int parentIndex);
	void renderTab(const Common::Array<byte> &data, int &selectedBlock);
	void renderTree(Common::Array<Block> &blocks, int &selectedBlock);
	void renderTreeNode(Common::Array<Block> &blocks, int index, int &selectedBlock);
	void renderBlock(const Common::Array<Block> &blocks, int selectedBlock, const Common::Array<byte> &data);

public:
	Explorer(const Common::Array<byte> &indexData, const Common::Array<byte> &mainData);

	void render(ImGuiID dockSpaceId, bool *open);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
