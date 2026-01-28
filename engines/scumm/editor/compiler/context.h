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

#ifndef SCUMM_EDITOR_COMPILER_CONTEXT_H
#define SCUMM_EDITOR_COMPILER_CONTEXT_H

#include "common/array.h"
#include "common/hash-str.h"
#include "common/hashmap.h"
#include "common/str.h"
#include "common/scummsys.h"
#include "scumm/editor/compiler/function.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

class Declaration;
class Function;

enum ContextType {
	CONTEXT_GAME,
	CONTEXT_ROOM,
	CONTEXT_FUNCTION,
	CONTEXT_THREAD,
	CONTEXT_INLINED,
	CONTEXT_BLOCK,
	CONTEXT_SWITCH,
	CONTEXT_VERB,
	CONTEXT_FOR,
	CONTEXT_WHILE,
	CONTEXT_DO_WHILE
};

enum SymbolType {
	SYMBOL_CONSTANT,
	SYMBOL_VARIABLE,
	SYMBOL_FUNCTION
};

class Context {
private:
	// Game constants (replacing Game::N_DEFAULT_ACTORS, Game::MAX_WORD_VARIABLES, Game::MAX_LOCAL_VARIABLES)
	static const uint16 N_DEFAULT_ACTORS = 12;
	static const uint16 MAX_WORD_VARIABLES = 8192;
	static const uint16 MAX_LOCAL_VARIABLES = 16;

	static const uint16 MIN_ACTOR;
	static const uint16 MIN_VERB;
	static const uint16 MIN_CLASS;
	static const uint16 MAX_CLASSES;

	static Common::Array<Context *> _instances;
	static uint16 _currentActor;
	static uint16 _currentVerb;
	static uint16 _currentClass;

	ContextType _type;
	Common::Array<Declaration *> *_declarations;
	Common::Array<Function *> *_functions;
	int32 _continueLabel;
	int32 _breakLabel;
	int32 _returnLabel;
	Common::HashMap<Common::String, uint32> _constantSymbols;
	Common::HashMap<Common::String, uint32> _variableSymbols;
	Common::HashMap<Common::String, uint32> _functionSymbols;

	static bool symbolExists(const Common::String &name);
	static bool isAddressUsed(uint32 address);

	void setConstantSymbols();
	void setVariableSymbols(bool fixedAddresses);
	void setFunctionSymbols();

public:
	static const uint16 LOCAL_VARIABLE_MASK = 0x4000;

	static uint32 labelCounter;
	static uint32 currentAddress;

	static void pushContext(Context *context);
	static void popContext();
	static bool resolveSymbol(const Common::String &symbol, uint32 &value, SymbolType &type);
	static Function *getFunction(const Common::String &functionName);
	static int32 getContinueLabel();
	static int32 getBreakLabel();
	static int32 getReturnLabel();
	static FunctionType getFunctionType();

	Context(ContextType type, Common::Array<Declaration *> *declarations, Common::Array<Function *> *functions, int32 continueLabel, int32 breakLabel, int32 returnLabel);
};

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm

#endif
