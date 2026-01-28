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

#include "scumm/editor/compiler/context.h"
#include "scumm/editor/compiler/compiler.h"
#include "scumm/editor/compiler/declaration.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

const uint16 Context::MIN_ACTOR = 1;
const uint16 Context::MIN_VERB = 1;
const uint16 Context::MIN_CLASS = 1;
const uint16 Context::MAX_CLASSES = 20;

Common::Array<Context *> Context::_instances;
uint16 Context::_currentActor;
uint16 Context::_currentVerb;
uint16 Context::_currentClass;

uint32 Context::labelCounter;
uint32 Context::currentAddress;

void Context::pushContext(Context *context) {
	_instances.push_back(context);

	// Reset actor and verb counters when parsing the complete game
	if (context->_type == CONTEXT_GAME) {
		_currentActor = MIN_ACTOR;
		_currentVerb = MIN_VERB;
		_currentClass = MIN_CLASS;
	}

	// When we enter functions, we have to reset the label counter and the current instruction address
	if (context->_type == CONTEXT_FUNCTION) {
		labelCounter = 0;
		currentAddress = 0;
	}

	// Compute the different kinds of symbols
	context->setConstantSymbols();
	context->setVariableSymbols(true);
	context->setVariableSymbols(false);
	context->setFunctionSymbols();
}

void Context::popContext() {
	_instances.pop_back();
}

bool Context::resolveSymbol(const Common::String &symbol, uint32 &value, SymbolType &type) {
	bool reachedInlinedContext = false;

	for (int i = _instances.size() - 1; i >= 0; i--)
		if (!reachedInlinedContext || _instances[i]->_type == CONTEXT_GAME || _instances[i]->_type == CONTEXT_ROOM) {
			// Check all the different symbol types
			if (_instances[i]->_constantSymbols.contains(symbol)) {
				value = _instances[i]->_constantSymbols[symbol];
				type = SYMBOL_CONSTANT;
				return true;
			}
			if (_instances[i]->_variableSymbols.contains(symbol)) {
				value = _instances[i]->_variableSymbols[symbol];
				type = SYMBOL_VARIABLE;
				return true;
			}
			if (_instances[i]->_functionSymbols.contains(symbol)) {
				value = _instances[i]->_functionSymbols[symbol];
				type = SYMBOL_FUNCTION;
				return true;
			}

			// When reaching an inlined function context here,
			// it means that if the symbol can be resolved, it
			// has to be global (room or game context).
			if (_instances[i]->_type == CONTEXT_INLINED)
				reachedInlinedContext = true;
		}
	return false;
}

Function *Context::getFunction(const Common::String &functionName) {
	for (int i = _instances.size() - 1; i >= 0; i--) {
		if (_instances[i]->_functions == nullptr)
			continue;

		for (uint j = 0; j < _instances[i]->_functions->size(); ++j)
			if ((*_instances[i]->_functions)[j]->getName() == functionName)
				return (*_instances[i]->_functions)[j];
	}
	return nullptr;
}

int32 Context::getContinueLabel() {
	for (int i = _instances.size() - 1; i >= 0; i--)
		if (_instances[i]->_continueLabel != -1)
			return _instances[i]->_continueLabel;
	return -1;
}

int32 Context::getBreakLabel() {
	for (int i = _instances.size() - 1; i >= 0; i--)
		if (_instances[i]->_breakLabel != -1)
			return _instances[i]->_breakLabel;
	return -1;
}

int32 Context::getReturnLabel() {
	for (int i = _instances.size() - 1; i >= 0; i--)
		if (_instances[i]->_returnLabel != -1)
			return _instances[i]->_returnLabel;
	return -1;
}

FunctionType Context::getFunctionType() {
	for (int i = _instances.size() - 1; i >= 0; i--)
		switch (_instances[i]->_type) {
		case CONTEXT_FUNCTION:
			return FUNCTION_NORMAL;
		case CONTEXT_THREAD:
			return FUNCTION_THREAD;
		case CONTEXT_INLINED:
			return FUNCTION_INLINED;
		default:
			break;
		}
	return FUNCTION_NORMAL;
}

bool Context::symbolExists(const Common::String &name) {
	bool reachedInlinedContext = false;

	for (int i = _instances.size() - 1; i >= 0; i--)
		if (!reachedInlinedContext || _instances[i]->_type == CONTEXT_GAME || _instances[i]->_type == CONTEXT_ROOM) {
			if (_instances[i]->_constantSymbols.contains(name))
				return true;
			if (_instances[i]->_variableSymbols.contains(name))
				return true;
			if (_instances[i]->_functionSymbols.contains(name))
				return true;

			// When reaching an inlined function context here,
			// it means that if the symbol exists, it has to
			// be global (room or game context).
			if (_instances[i]->_type == CONTEXT_INLINED)
				reachedInlinedContext = true;
		}
	return false;
}

bool Context::isAddressUsed(uint32 address) {
	for (int i = _instances.size() - 1; i >= 0; i--) {
		Common::HashMap<Common::String, uint32>::const_iterator it;
		for (it = _instances[i]->_variableSymbols.begin(); it != _instances[i]->_variableSymbols.end(); ++it)
			if (it->_value == address)
				return true;
	}
	return false;
}

Context::Context(ContextType type, Common::Array<Declaration *> *declarations, Common::Array<Function *> *functions, int32 continueLabel, int32 breakLabel, int32 returnLabel)
	: _type(type),
	  _declarations(declarations),
	  _functions(functions),
	  _continueLabel(continueLabel),
	  _breakLabel(breakLabel),
	  _returnLabel(returnLabel) {
}

void Context::setConstantSymbols() {
	if (_declarations == nullptr)
		return;

	// We first add normal constants, then actors and verbs which are treated as constants also
	for (uint i = 0; i < _declarations->size(); ++i)
		if ((*_declarations)[i]->getType() == DECLARATION_CONST) {
			Common::String name = (*_declarations)[i]->getName();
			if (symbolExists(name))
				Compiler::log(LOG_ERROR,"Symbol \"%s\" already declared", name.c_str());
			_constantSymbols[name] = (*_declarations)[i]->getValue();
		} else if ((*_declarations)[i]->getType() == DECLARATION_ACTOR) {
			Common::String name = (*_declarations)[i]->getName();
			if (symbolExists(name))
				Compiler::log(LOG_ERROR,"Symbol \"%s\" already declared", name.c_str());
			_constantSymbols[name] = _currentActor++;
			if (_currentActor > N_DEFAULT_ACTORS)
				Compiler::log(LOG_ERROR,"Too many actors declared");
		} else if ((*_declarations)[i]->getType() == DECLARATION_VERB) {
			Common::String name = (*_declarations)[i]->getName();
			if (symbolExists(name))
				Compiler::log(LOG_ERROR,"Symbol \"%s\" already declared", name.c_str());
			_constantSymbols[name] = _currentVerb++;
		} else if ((*_declarations)[i]->getType() == DECLARATION_CLASS) {
			if (_currentClass > MAX_CLASSES)
				Compiler::log(LOG_ERROR,"Too many classes declared");
			Common::String name = (*_declarations)[i]->getName();
			if (symbolExists(name))
				Compiler::log(LOG_ERROR,"Symbol \"%s\" already declared", name.c_str());
			_constantSymbols[name] = _currentClass++;
		}
}

void Context::setVariableSymbols(bool fixedAddresses) {
	// No need to look for variables with fixed addresses when the context type is not global
	if (_declarations == nullptr || ((_type != CONTEXT_GAME) && fixedAddresses))
		return;

	for (uint i = 0; i < _declarations->size(); ++i)
		if ((*_declarations)[i]->getType() == DECLARATION_VAR) {
			if ((*_declarations)[i]->hasFixedAddress() != fixedAddresses) {
				if (_type != CONTEXT_GAME)
					Compiler::log(LOG_ERROR,"Cannot specify the address of local variable");
				continue;
			}

			Common::String name = (*_declarations)[i]->getName();
			uint32 address;

			// Check if symbol doesn't exist already
			if (symbolExists(name))
				Compiler::log(LOG_ERROR,"Symbol \"%s\" already declared", name.c_str());

			// Check if address is not already taken in case it's a fixed address
			if (fixedAddresses) {
				address = (*_declarations)[i]->getValue();
				if (isAddressUsed(address))
					Compiler::log(LOG_ERROR,"Symbol \"%s\" can't be mapped as this address is already used", name.c_str());
			}
			// Search through the map to get the smallest available address
			else {
				address = (_type == CONTEXT_GAME) ? 0 : LOCAL_VARIABLE_MASK;
				while (isAddressUsed(address))
					address++;
			}

			// Check whether the address fits in the authorized range
			// (and make sure we reserve the last local variable for temporary arrays)
			if ((_type == CONTEXT_GAME && address >= MAX_WORD_VARIABLES)
			|| (_type != CONTEXT_GAME && (address < LOCAL_VARIABLE_MASK || address >= ((MAX_LOCAL_VARIABLES - 1) | LOCAL_VARIABLE_MASK))))
				Compiler::log(LOG_ERROR,"Symbol \"%s\" can't be mapped as this address is invalid", name.c_str());
			_variableSymbols[name] = address;
		}
}

void Context::setFunctionSymbols() {
	if (_functions == nullptr)
		return;

	for (uint i = 0; i < _functions->size(); ++i) {
		Common::String name = (*_functions)[i]->getName();
		if (symbolExists(name))
			Compiler::log(LOG_ERROR,"Function \"%s\" already declared", name.c_str());
		_functionSymbols[name] = (*_functions)[i]->getID();
	}
}

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm
