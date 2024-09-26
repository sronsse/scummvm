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

#ifndef SCUMM_EDITOR_SCREEN_H
#define SCUMM_EDITOR_SCREEN_H

#include "backends/imgui/imgui.h"

namespace Scumm {

class ScummEngine;

namespace Editor {

class Screen {
private:
	static const int kNumLayers = 4;  // Main, Text, Verb virtual screens + subtitles overlay

	ScummEngine *_engine;
	void *_textures[kNumLayers];
	bool _layerVisible[kNumLayers];
	int _scale;

public:
	Screen(ScummEngine *engine);
	~Screen();

	void render(ImGuiID dockSpaceId, bool *open);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
