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

#ifndef SCUMM_EDITOR_CHARSET_H
#define SCUMM_EDITOR_CHARSET_H

#include "backends/imgui/imgui.h"

#include "scumm/editor/resource.h"

namespace Scumm {

namespace Editor {

class Charset {
private:
	DCHR &_dchr;
	LECF &_lecf;

	int _selectedCharset;
	int _selectedChar;
	int _selectedPaletteRoom;  // -1 means use charset's room
	void *_texture;

	CHR *findCharset(int charsetId);
	LFLF *findLFLF(int charsetId);
	LFLF *findLFLFByRoom(int roomNum);
	void *createCharsetTexture(CHR *chr, LFLF *lflf);
	void drawCharsetPreview(ImDrawList *drawList, ImVec2 pos, int scale, CHR *chr, LFLF *lflf, bool transparent);
	void drawCharacter(ImDrawList *drawList, ImVec2 pos, int scale, CHR *chr, LFLF *lflf, int charIndex, bool transparent);
	void renderCharsetTooltip(int charsetId);
	void renderCharacterTooltip(CHR *chr, LFLF *lflf, int charIndex);
	void renderCharacterPreview(CHR *chr, LFLF *lflf, int charIndex);

	void renderCharsetList();
	void renderCharsetProperties();
	void renderPalette(CHR *chr, LFLF *lflf);
	void renderCharacters(CHR *chr, LFLF *lflf);
	void renderCharsetImage(CHR *chr, LFLF *lflf);

public:
	Charset(DCHR &dchr, LECF &lecf);
	~Charset();

	void render(ImGuiID dockSpaceId, bool *open);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
