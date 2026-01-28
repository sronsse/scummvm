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

#ifndef SCUMM_EDITOR_COMPILER_DECLARATION_H
#define SCUMM_EDITOR_COMPILER_DECLARATION_H

#include "common/str.h"
#include "common/scummsys.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

enum DeclarationType {
	DECLARATION_CONST,
	DECLARATION_VAR,
	DECLARATION_ACTOR,
	DECLARATION_VERB,
	DECLARATION_CLASS
};

class Declaration {
private:
	DeclarationType _type;
	Common::String _name;
	uint32 _value;
	bool _fixedAddress;

public:
	Declaration(DeclarationType type, const Common::String &name);
	Declaration(DeclarationType type, const Common::String &name, uint32 value);

	DeclarationType getType() { return _type; }
	Common::String getName() { return _name; }
	uint32 getValue() { return _value; }
	bool hasFixedAddress() { return _fixedAddress; }
};

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm

#endif
