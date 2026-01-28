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

#include "scumm/editor/compiler/declaration.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

Declaration::Declaration(DeclarationType type, const Common::String &name)
	: _type(type),
	  _name(name),
	  _value(0),
	  _fixedAddress(false) {
}

Declaration::Declaration(DeclarationType type, const Common::String &name, uint32 value)
	: _type(type),
	  _name(name),
	  _value(value),
	  _fixedAddress(type == DECLARATION_VAR) {
}

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm
