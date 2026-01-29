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

#ifndef SCUMM_EDITOR_STYLE_H
#define SCUMM_EDITOR_STYLE_H

#include "backends/imgui/IconsMaterialSymbols.h"
#include "backends/imgui/imgui.h"

// Icons
#define ICON_EDITOR    ICON_MS_CONSTRUCTION
#define ICON_GAME      ICON_MS_VIDEOGAME_ASSET
#define ICON_RESOURCE  ICON_MS_DATABASE
#define ICON_SCREEN    ICON_MS_MONITOR

// Solarized Dark palette
#define SOL_BASE03  ImVec4(0.000f, 0.169f, 0.212f, 1.0f)
#define SOL_BASE02  ImVec4(0.027f, 0.212f, 0.259f, 1.0f)
#define SOL_BASE01  ImVec4(0.345f, 0.431f, 0.459f, 1.0f)
#define SOL_BASE00  ImVec4(0.396f, 0.482f, 0.514f, 1.0f)
#define SOL_BASE0   ImVec4(0.514f, 0.580f, 0.588f, 1.0f)
#define SOL_BASE1   ImVec4(0.576f, 0.631f, 0.631f, 1.0f)
#define SOL_YELLOW  ImVec4(0.710f, 0.537f, 0.000f, 1.0f)
#define SOL_ORANGE  ImVec4(0.796f, 0.294f, 0.086f, 1.0f)
#define SOL_RED     ImVec4(0.863f, 0.196f, 0.184f, 1.0f)
#define SOL_MAGENTA ImVec4(0.827f, 0.212f, 0.510f, 1.0f)
#define SOL_VIOLET  ImVec4(0.424f, 0.443f, 0.769f, 1.0f)
#define SOL_BLUE    ImVec4(0.149f, 0.545f, 0.824f, 1.0f)
#define SOL_CYAN    ImVec4(0.165f, 0.631f, 0.596f, 1.0f)
#define SOL_GREEN   ImVec4(0.522f, 0.600f, 0.000f, 1.0f)

#endif
