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

#ifndef SCUMM_EDITOR_COMPILER_FUNCTION_H
#define SCUMM_EDITOR_COMPILER_FUNCTION_H

#include "common/array.h"
#include "common/debug.h"
#include "common/hash-str.h"
#include "common/hashmap.h"
#include "common/str.h"
#include "common/scummsys.h"
#include "common/textconsole.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

class BlockStatement;
class Declaration;
class Instruction;

enum FunctionType {
	FUNCTION_NORMAL,
	FUNCTION_THREAD,
	FUNCTION_INLINED
};

class Function {
private:
	FunctionType _type;
	Common::String _name;
	uint16 _id;
	Common::Array<Declaration *> _arguments;
	BlockStatement *_blockStatement;
	Common::Array<Instruction *> _instructions;
	Common::Array<byte> _byteCode;

	void removeLabels();

public:
	Function(FunctionType type, const Common::String &name, BlockStatement *blockS);
	void compile();
	FunctionType getType() { return _type; }
	Common::String getName() { return _name; }
	uint16 getID() { return _id; }
	void setID(uint16 id) { _id = id; }
	void addArgument(Declaration *d) { _arguments.push_back(d); }
	byte getNumberOfArguments() { return _arguments.size(); }
	Declaration *getArgument(byte index) { return _arguments[index]; }
	BlockStatement *getBlockStatement() { return _blockStatement; }
	uint32 getNumberOfInstructions() { return _instructions.size(); }
	Instruction *getInstruction(uint32 index) { return _instructions[index]; }
	uint32 getNumberOfBytes() { return _byteCode.size(); }
	byte getByte(uint32 index) { return _byteCode[index]; }
	~Function();
};

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm

#endif
