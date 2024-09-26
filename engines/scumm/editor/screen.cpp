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

#include "common/system.h"

#include "graphics/paletteman.h"
#include "graphics/surface.h"

#include "scumm/scumm.h"

#include "scumm/editor/screen.h"
#include "scumm/editor/style.h"

namespace Scumm {

namespace Editor {

static const char *layerNames[] = { "Main", "Text", "Verb", "Subtitles" };
static const int subtitlesLayer = 3;

Screen::Screen(ScummEngine *engine) : _engine(engine), _scale(2) {
	for (int i = 0; i < kNumLayers; i++) {
		_textures[i] = nullptr;
		_layerVisible[i] = true;
	}
}

Screen::~Screen() {
	for (int i = 0; i < kNumLayers; i++) {
		if (_textures[i])
			g_system->freeImGuiTexture(_textures[i]);
	}
}

void Screen::render(ImGuiID dockSpaceId, bool *open) {
	ImGui::SetNextWindowDockID(dockSpaceId, ImGuiCond_FirstUseEver);
	ImGui::Begin(ICON_SCREEN " Screen", open);

	// Scale slider and layer visibility checkboxes
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("Scale", &_scale, 1, 8, "%dx");

	for (int i = 0; i < kNumLayers; i++) {
		ImGui::SameLine();
		ImGui::Checkbox(layerNames[i], &_layerVisible[i]);
	}

	// Grab palette
	byte palette[256 * 3];
	g_system->getPaletteManager()->grabPalette(palette, 0, 256);

	// Get screen dimensions
	int screenWidth = _engine->_screenWidth;
	int screenHeight = _engine->_screenHeight;

	if (screenWidth <= 0 || screenHeight <= 0) {
		ImGui::TextColored(SOL_BASE01, "No screen data available.");
		ImGui::End();
		return;
	}

	// Free and recreate textures for each layer
	for (int i = 0; i < kNumLayers; i++) {
		if (_textures[i]) {
			g_system->freeImGuiTexture(_textures[i]);
			_textures[i] = nullptr;
		}

		if (!_layerVisible[i])
			continue;

		// Subtitles overlay
		if (i == subtitlesLayer) {
			const Graphics::Surface &textSurf = _engine->_textSurface;
			if (textSurf.w <= 0 || textSurf.h <= 0)
				continue;

			// Create RGBA surface with alpha
			Graphics::Surface rgbaSurface;
			rgbaSurface.create(textSurf.w, textSurf.h, Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24));

			bool hasContent = false;
			for (int y = 0; y < textSurf.h; y++) {
				const byte *srcRow = (const byte *)textSurf.getBasePtr(0, y);
				uint32 *dstRow = (uint32 *)rgbaSurface.getBasePtr(0, y);
				for (int x = 0; x < textSurf.w; x++) {
					byte colorIdx = srcRow[x];
					if (colorIdx == 0xFD) {
						dstRow[x] = 0x00000000;
					} else {
						hasContent = true;
						byte r = palette[colorIdx * 3 + 0];
						byte g = palette[colorIdx * 3 + 1];
						byte b = palette[colorIdx * 3 + 2];
						dstRow[x] = (0xFF << 24) | (b << 16) | (g << 8) | r;
					}
				}
			}

			if (hasContent)
				_textures[i] = g_system->getImGuiTexture(rgbaSurface, nullptr, 0);

			rgbaSurface.free();
			continue;
		}

		// Virtual screens (Main, Text, Verb)
		VirtScreen &vs = _engine->_virtscr[i];
		if (vs.w <= 0 || vs.h <= 0)
			continue;

		Graphics::Surface scrolledSurface;
		scrolledSurface.create(vs.w, vs.h, vs.format);

		int bpp = vs.format.bytesPerPixel;
		for (int y = 0; y < vs.h; y++) {
			const byte *srcRow = vs.getPixels(0, y);
			byte *dstRow = (byte *)scrolledSurface.getBasePtr(0, y);
			memcpy(dstRow, srcRow, vs.w * bpp);
		}

		_textures[i] = g_system->getImGuiTexture(scrolledSurface, palette, 256);
		scrolledSurface.free();
	}

	// Scrollable preview area
	ImGui::BeginChild("ScreenPreview", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	ImDrawList *drawList = ImGui::GetWindowDrawList();

	// Draw background
	ImVec2 bgP1 = canvasPos;
	ImVec2 bgP2(canvasPos.x + screenWidth * _scale, canvasPos.y + screenHeight * _scale);
	drawList->AddRectFilled(bgP1, bgP2, IM_COL32(0, 0, 0, 255));

	// Render each visible layer
	for (int i = 0; i < kNumLayers; i++) {
		if (!_layerVisible[i] || !_textures[i])
			continue;

		float layerX, layerY, layerW, layerH;

		if (i == subtitlesLayer) {
			const Graphics::Surface &textSurf = _engine->_textSurface;
			if (textSurf.w <= 0 || textSurf.h <= 0)
				continue;
			layerX = canvasPos.x;
			layerY = canvasPos.y;
			layerW = textSurf.w * _scale;
			layerH = textSurf.h * _scale;
		} else {
			VirtScreen &vs = _engine->_virtscr[i];
			if (vs.w <= 0 || vs.h <= 0)
				continue;
			layerX = canvasPos.x;
			layerY = canvasPos.y + vs.topline * _scale;
			layerW = vs.w * _scale;
			layerH = vs.h * _scale;
		}

		ImVec2 p1(layerX, layerY);
		ImVec2 p2(layerX + layerW, layerY + layerH);
		drawList->AddImage((ImTextureID)(intptr_t)_textures[i], p1, p2);
	}

	// Reserve space for canvas
	ImGui::Dummy(ImVec2((float)screenWidth * _scale, (float)screenHeight * _scale));

	ImGui::EndChild();
	ImGui::End();
}

} // End of namespace Editor

} // End of namespace Scumm
