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

#include "common/scummsys.h"

#include "backends/events/sdl/finger-sdl-events.h"
#include "backends/platform/sdl/sdl.h"
#include "engines/engine.h"

#include "common/util.h"
#include "common/events.h"
#include "common/config-manager.h"

#include "math.h"

FingerSdlEventSource::FingerSdlEventSource() {
	for (int port = 0; port < MAX_NUM_PORTS; port++) {
		for (int i = 0; i < MAX_NUM_FINGERS; i++) {
			_finger[port][i].id = -1;
			_finger[port][i].timeLastDown = 0;
			_finger[port][i].lastX = 0;
			_finger[port][i].lastY = 0;
			_finger[port][i].lastDownX = 0;
			_finger[port][i].lastDownY = 0;
		}
		_multiFingerDragging[port] = DRAG_NONE;
	}

	for (int port = 0; port < MAX_NUM_PORTS; port++) {
		for (int i = 0; i < 2; i++) {
			_simulatedClickStartTime[port][i] = 0;
		}
	}

	_hiresDX = 0;
	_hiresDY = 0;

#if SDL_VERSION_ATLEAST(2,0,10)
	// ensure that touch doesn't create double-events
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
#endif
}

bool FingerSdlEventSource::pollEvent(Common::Event &event) {
	finishSimulatedMouseClicks();
	return SdlEventSource::pollEvent(event);
}

void FingerSdlEventSource::preprocessEvents(SDL_Event *event) {

	// Supported touch gestures:
	// left mouse click: single finger short tap
	// right mouse click: second finger short tap while first finger is still down
	// pointer motion: single finger drag
	if (event->type == SDL_FINGERDOWN || event->type == SDL_FINGERUP || event->type == SDL_FINGERMOTION) {
		// front (0) or back (1) panel
		SDL_TouchID port = event->tfinger.touchId;
		if (port < MAX_NUM_PORTS && port >= 0) {
			// touchpad_mouse_mode off: use only front panel for direct touch control of pointer
			// touchpad_mouse_mode on: also enable rear touch with indirect touch control
			// where the finger can be somewhere else than the pointer and still move it
			if (isPortActive(port)) {
				switch (event->type) {
				case SDL_FINGERDOWN:
					preprocessFingerDown(event);
					break;
				case SDL_FINGERUP:
					preprocessFingerUp(event);
					break;
				case SDL_FINGERMOTION:
					preprocessFingerMotion(event);
					break;
				}
			}
		}
	}
}

void FingerSdlEventSource::preprocessFingerDown(SDL_Event *event) {
	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;
	// id (for multitouch)
	SDL_FingerID id = event->tfinger.fingerId;

	int x = _mouseX;
	int y = _mouseY;

	if (port >= MAX_NUM_PORTS) {
		return;
	}

	if (!isTouchpadMode(port)) {
		convertTouchXYToGameXY(event->tfinger.x, event->tfinger.y, &x, &y);
	}

	// make sure each finger is not reported down multiple times
	for (int i = 0; i < MAX_NUM_FINGERS; i++) {
		if (_finger[port][i].id == id) {
			_finger[port][i].id = -1;
		}
	}

	// we need the timestamps to decide later if the user performed a short tap (click)
	// or a long tap (drag)
	// we also need the last coordinates for each finger to keep track of dragging
	for (int i = 0; i < MAX_NUM_FINGERS; i++) {
		if (_finger[port][i].id == -1) {
			_finger[port][i].id = id;
			_finger[port][i].timeLastDown = event->tfinger.timestamp;
			_finger[port][i].lastDownX = event->tfinger.x;
			_finger[port][i].lastDownY = event->tfinger.y;
			_finger[port][i].lastX = x;
			_finger[port][i].lastY = y;
			break;
		}
	}
}

void FingerSdlEventSource::preprocessFingerUp(SDL_Event *event) {
	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;
	// id (for multitouch)
	SDL_FingerID id = event->tfinger.fingerId;

	if (port >= MAX_NUM_PORTS) {
		return;
	}

	// find out how many fingers were down before this event
	int numFingersDown = 0;
	for (int i = 0; i < MAX_NUM_FINGERS; i++) {
		if (_finger[port][i].id >= 0) {
			numFingersDown++;
		}
	}

	int x = _mouseX;
	int y = _mouseY;

	for (int i = 0; i < MAX_NUM_FINGERS; i++) {
		if (_finger[port][i].id == id) {
			_finger[port][i].id = -1;
			if (!_multiFingerDragging[port]) {
				if ((event->tfinger.timestamp - _finger[port][i].timeLastDown) <= MAX_TAP_TIME) {
					// short (<MAX_TAP_TIME ms) tap is interpreted as right/left mouse click depending on # fingers already down
					// but only if the finger hasn't moved since it was pressed down by more than MAX_TAP_MOTION_DISTANCE pixels
				  	Common::Point touchscreenSize = getTouchscreenSize();
					float xrel = ((event->tfinger.x * (float) touchscreenSize.x) - (_finger[port][i].lastDownX * (float) touchscreenSize.x));
					float yrel = ((event->tfinger.y * (float) touchscreenSize.y) - (_finger[port][i].lastDownY * (float) touchscreenSize.y));
					float maxRSquared = (float) (MAX_TAP_MOTION_DISTANCE * MAX_TAP_MOTION_DISTANCE);
					if ((xrel * xrel + yrel * yrel) < maxRSquared) {
						if (numFingersDown == 2 || numFingersDown == 1) {
							Uint8 simulatedButton = 0;
							if (numFingersDown == 2) {
								simulatedButton = SDL_BUTTON_RIGHT;
								// need to raise the button later
								_simulatedClickStartTime[port][1] = event->tfinger.timestamp;
							} else if (numFingersDown == 1) {
								simulatedButton = SDL_BUTTON_LEFT;
								// need to raise the button later
								_simulatedClickStartTime[port][0] = event->tfinger.timestamp;
								if (!isTouchpadMode(port)) {
									convertTouchXYToGameXY(event->tfinger.x, event->tfinger.y, &x, &y);
								}
							}

							event->type = SDL_MOUSEBUTTONDOWN;
							event->button.button = simulatedButton;
							event->button.x = x;
							event->button.y = y;
						}
					}
				}
			} else if (numFingersDown == 1) {
				// when dragging, and the last finger is lifted, the drag is over
				if (!isTouchpadMode(port)) {
					convertTouchXYToGameXY(event->tfinger.x, event->tfinger.y, &x, &y);
				}
				Uint8 simulatedButton = 0;
				if (_multiFingerDragging[port] == DRAG_THREE_FINGER)
					simulatedButton = SDL_BUTTON_RIGHT;
				else {
					simulatedButton = SDL_BUTTON_LEFT;
				}
				event->type = SDL_MOUSEBUTTONUP;
				event->button.button = simulatedButton;
				event->button.x = x;
				event->button.y = y;
				_multiFingerDragging[port] = DRAG_NONE;
			}
		}
	}
}

void FingerSdlEventSource::preprocessFingerMotion(SDL_Event *event) {
	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;
	// id (for multitouch)
	SDL_FingerID id = event->tfinger.fingerId;

	if (port >= MAX_NUM_PORTS) {
		return;
	}

	// find out how many fingers were down before this event
	int numFingersDown = 0;
	for (int i = 0; i < MAX_NUM_FINGERS; i++) {
		if (_finger[port][i].id >= 0) {
			numFingersDown++;
		}
	}

	if (numFingersDown >= 1) {
		int x = _mouseX;
		int y = _mouseY;
		int xMax = _graphicsManager->getWindowWidth() - 1;
		int yMax = _graphicsManager->getWindowHeight() - 1;

		if (!isTouchpadMode(port)) {
			convertTouchXYToGameXY(event->tfinger.x, event->tfinger.y, &x, &y);
		}	else {
			// for relative mode, use the pointer speed setting
			float speedFactor = 1.0;

			switch (ConfMan.getInt("kbdmouse_speed")) {
			// 0.25 keyboard pointer speed
			case 0:
				speedFactor = 0.25;
				break;
			// 0.5 speed
			case 1:
				speedFactor = 0.5;
				break;
			// 0.75 speed
			case 2:
				speedFactor = 0.75;
				break;
			// 1.0 speed
			case 3:
				speedFactor = 1.0;
				break;
			// 1.25 speed
			case 4:
				speedFactor = 1.25;
				break;
			// 1.5 speed
			case 5:
				speedFactor = 1.5;
				break;
			// 1.75 speed
			case 6:
				speedFactor = 1.75;
				break;
			// 2.0 speed
			case 7:
				speedFactor = 2.0;
				break;
			default:
				speedFactor = 1.0;
			}

			// convert touch events to relative mouse pointer events
			// track sub-pixel relative finger motion using the FINGER_SUBPIXEL_MULTIPLIER
			_hiresDX += (event->tfinger.dx * 1.25 * speedFactor * xMax * FINGER_SUBPIXEL_MULTIPLIER);
			_hiresDY += (event->tfinger.dy * 1.25 * speedFactor * yMax * FINGER_SUBPIXEL_MULTIPLIER);
			int xRel = _hiresDX / FINGER_SUBPIXEL_MULTIPLIER;
			int yRel = _hiresDY / FINGER_SUBPIXEL_MULTIPLIER;
			x = _mouseX + xRel;
			y = _mouseY + yRel;
			_hiresDX %= FINGER_SUBPIXEL_MULTIPLIER;
			_hiresDY %= FINGER_SUBPIXEL_MULTIPLIER;
		}

		x = CLIP(x, 0, xMax);
		y = CLIP(y, 0, yMax);

		// update the current finger's coordinates so we can track it later
		for (int i = 0; i < MAX_NUM_FINGERS; i++) {
			if (_finger[port][i].id == id) {
				_finger[port][i].lastX = x;
				_finger[port][i].lastY = y;
			}
		}

		// If we are starting a multi-finger drag, start holding down the mouse button
		if (numFingersDown >= 2) {
			if (!_multiFingerDragging[port]) {
				// only start a multi-finger drag if at least two fingers have been down long enough
				int numFingersDownLong = 0;
				for (int i = 0; i < MAX_NUM_FINGERS; i++) {
					if (_finger[port][i].id >= 0) {
						if (event->tfinger.timestamp - _finger[port][i].timeLastDown > MAX_TAP_TIME) {
							numFingersDownLong++;
						}
					}
				}
				if (numFingersDownLong >= 2) {
					// starting drag, so push mouse down at current location (back)
					// or location of "oldest" finger (front)
					int mouseDownX = _mouseX;
					int mouseDownY = _mouseY;
					if (!isTouchpadMode(port)) {
						for (int i = 0; i < MAX_NUM_FINGERS; i++) {
							if (_finger[port][i].id == id) {
								Uint32 earliestTime = _finger[port][i].timeLastDown;
								for (int j = 0; j < MAX_NUM_FINGERS; j++) {
									if (_finger[port][j].id >= 0 && (i != j) ) {
										if (_finger[port][j].timeLastDown < earliestTime) {
											mouseDownX = _finger[port][j].lastX;
											mouseDownY = _finger[port][j].lastY;
											earliestTime = _finger[port][j].timeLastDown;
										}
									}
								}
								break;
							}
						}
					}
					Uint8 simulatedButton = 0;
					if (numFingersDownLong == 2) {
						simulatedButton = SDL_BUTTON_LEFT;
						_multiFingerDragging[port] = DRAG_TWO_FINGER;
					} else {
						simulatedButton = SDL_BUTTON_RIGHT;
						_multiFingerDragging[port] = DRAG_THREE_FINGER;
					}
					SDL_Event ev;
					ev.type = SDL_MOUSEBUTTONDOWN;
					ev.button.button = simulatedButton;
					ev.button.x = mouseDownX;
					ev.button.y = mouseDownY;
					SDL_PushEvent(&ev);
				}
			}
		}

		//check if this is the "oldest" finger down (or the only finger down), otherwise it will not affect mouse motion
		bool updatePointer = true;
		if (numFingersDown > 1) {
			for (int i = 0; i < MAX_NUM_FINGERS; i++) {
				if (_finger[port][i].id == id) {
					for (int j = 0; j < MAX_NUM_FINGERS; j++) {
						if (_finger[port][j].id >= 0 && (i != j) ) {
							if (_finger[port][j].timeLastDown < _finger[port][i].timeLastDown) {
								updatePointer = false;
							}
						}
					}
				}
			}
		}
		if (updatePointer) {
			event->type = SDL_MOUSEMOTION;
			event->motion.x = x;
			event->motion.y = y;
		}
	}
}

void FingerSdlEventSource::convertTouchXYToGameXY(float touchX, float touchY, int *gameX, int *gameY) {
	int screenH = _graphicsManager->getWindowHeight();
	int screenW = _graphicsManager->getWindowWidth();
	Common::Point touchscreenSize = getTouchscreenSize();

	const int dispW = touchscreenSize.x;
	const int dispH = touchscreenSize.y;

	int x, y, w, h;
	float sx, sy;
	float ratio = (float)screenW / (float)screenH;

	h = dispH;
	w = h * ratio;

	x = (dispW - w) / 2;
	y = (dispH - h) / 2;

	sy = (float)h / (float)screenH;
	sx = (float)w / (float)screenW;

	// Find touch coordinates in terms of screen pixels
	float dispTouchX = (touchX * (float)dispW);
	float dispTouchY = (touchY * (float)dispH);

	*gameX = CLIP((int)((dispTouchX - x) / sx), 0, screenW - 1);
	*gameY = CLIP((int)((dispTouchY - y) / sy), 0, screenH - 1);
}

void FingerSdlEventSource::finishSimulatedMouseClicks() {
	for (int port = 0; port < MAX_NUM_PORTS; port++) {
		for (int i = 0; i < 2; i++) {
			if (_simulatedClickStartTime[port][i] != 0) {
				Uint32 currentTime = SDL_GetTicks();
				if (currentTime - _simulatedClickStartTime[port][i] >= SIMULATED_CLICK_DURATION) {
					int simulatedButton;
					if (i == 0) {
						simulatedButton = SDL_BUTTON_LEFT;
					} else {
						simulatedButton = SDL_BUTTON_RIGHT;
					}
					SDL_Event ev;
					ev.type = SDL_MOUSEBUTTONUP;
					ev.button.button = simulatedButton;
					ev.button.x = _mouseX;
					ev.button.y = _mouseY;
					SDL_PushEvent(&ev);

					_simulatedClickStartTime[port][i] = 0;
				}
			}
		}
	}
}
