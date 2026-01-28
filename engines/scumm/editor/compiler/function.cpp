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

#include "scumm/editor/compiler/function.h"
#include "scumm/editor/compiler/compiler.h"
#include "scumm/editor/compiler/context.h"
#include "scumm/editor/compiler/declaration.h"
#include "scumm/editor/compiler/instruction.h"
#include "scumm/editor/compiler/statement.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

Function::Function(FunctionType type, const Common::String &name, BlockStatement *blockS)
	: _type(type),
	  _name(name),
	  _id(0),
	  _blockStatement(blockS) {
}

void Function::removeLabels() {
	// First, we get the label instruction addresses and remove the corresponding instructions
	Common::HashMap<uint32, uint32> labelAddresses;
	uint i = 0;
	while (i < _instructions.size()) {
		if (_instructions[i]->getType() == INSTRUCTION_LABEL) {
			labelAddresses[_instructions[i]->getLabel()] = _instructions[i]->getAddress();
			delete _instructions[i];
			_instructions.remove_at(i);
		} else {
			i++;
		}
	}

	// Then, we replace labels by relative offsets
	for (uint j = 0; j < _instructions.size(); ++j) {
		if (_instructions[j]->getType() == INSTRUCTION_VALUE && _instructions[j]->getValue().hasPrefix("LABEL_")) {
			uint32 label;
			sscanf(_instructions[j]->getValue().c_str() + 6, "%u", &label);
			// The magic 2 in this statement is because SCUMM jump/if/ifNot instructions
			// don't take the offset following them into account
			int32 offset = (int32)labelAddresses[label] - (int32)_instructions[j]->getAddress() - 2;
			Common::String offsetStr = Common::String::format("%d", offset);
			_instructions[j]->setValue(offsetStr);
		}
	}
}

void Function::compile() {
	Compiler::log(LOG_DEBUG,"Compiling function \"%s\"...", _name.c_str());

	ContextType contextType;
	switch (_type) {
	case FUNCTION_NORMAL:
		contextType = CONTEXT_FUNCTION;
		break;
	case FUNCTION_THREAD:
		contextType = CONTEXT_THREAD;
		break;
	case FUNCTION_INLINED:
		Compiler::log(LOG_ERROR,"Can't compile inline functions");
		return;
	}

	Context context(contextType, &_arguments, nullptr, -1, -1, 0);
	Context::pushContext(&context);

	// Prepare labels first
	Context::labelCounter++;

	// Compile block statement
	_blockStatement->compile(_instructions);

	// Return label (should always be LABEL_0)
	_instructions.push_back(new Instruction(0));

	Context::popContext();

	// stopObjectCode2 instruction
	_instructions.push_back(new Instruction("stopObjectCode2"));

	// Replace labels by relative offsets
	removeLabels();

	// Assemble instructions
	for (uint i = 0; i < _instructions.size(); ++i)
		_instructions[i]->assemble(_byteCode);
}

Function::~Function() {
	for (uint i = 0; i < _arguments.size(); ++i)
		delete _arguments[i];
	delete _blockStatement;
	for (uint i = 0; i < _instructions.size(); ++i)
		delete _instructions[i];
}

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm
