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

#include "scumm/editor/compiler/expression.h"
#include "scumm/editor/compiler/compiler.h"
#include "scumm/editor/compiler/context.h"
#include "scumm/editor/compiler/declaration.h"
#include "scumm/editor/compiler/function.h"
#include "scumm/editor/compiler/instruction.h"
#include "scumm/editor/compiler/statement.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

const byte StringExpression::STRING_OPCODE = 0xFF;
const byte StringExpression::INT_MESSAGE = 4;
const byte StringExpression::VERB_MESSAGE = 5;
const byte StringExpression::NAME_MESSAGE = 6;
const byte StringExpression::STRING_MESSAGE = 7;
const byte StringExpression::VOICE_MESSAGE = 10;

Expression *Expression::simplifyUnaryExpression(ExpressionType type, Expression *e) {
	if (e->getType() != EXPRESSION_CONSTANT)
		return new UnaryExpression(type, e);

	ConstantExpression *result;
	switch (type) {
	case EXPRESSION_UMINUS:
		result = new ConstantExpression(-((ConstantExpression *)e)->getNumber());
		break;
	case EXPRESSION_NOT:
		result = new ConstantExpression(!((ConstantExpression *)e)->getNumber());
		break;
	default:
		result = nullptr;
	}

	delete e;
	return result;
}

Expression *Expression::simplifyBinaryExpression(ExpressionType type, Expression *e1, Expression *e2) {
	if (e1->getType() != EXPRESSION_CONSTANT || e2->getType() != EXPRESSION_CONSTANT)
		return new BinaryExpression(type, e1, e2);

	ConstantExpression *result;
	switch (type) {
	case EXPRESSION_ADD:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() + ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_SUB:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() - ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_DIV:
		if (((ConstantExpression *)e2)->getNumber() == 0) {
			Compiler::log(LOG_ERROR, "Division by zero");
			delete e1;
			delete e2;
			return new ConstantExpression(0);
		}
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() / ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_MUL:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() * ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_LT:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() < ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_GT:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() > ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_BAND:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() & ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_BOR:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() | ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_LE:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() <= ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_GE:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() >= ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_NE:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() != ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_EQ:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() == ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_LAND:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() && ((ConstantExpression *)e2)->getNumber());
		break;
	case EXPRESSION_LOR:
		result = new ConstantExpression(((ConstantExpression *)e1)->getNumber() || ((ConstantExpression *)e2)->getNumber());
		break;
	default:
		result = nullptr;
	}

	delete e1;
	delete e2;
	return result;
}

VariableExpression::VariableExpression(const Common::String &identifier)
	: AssignableExpression(EXPRESSION_VARIABLE, identifier) {
}

void VariableExpression::compile(Common::Array<Instruction *> &instructions) {
	uint32 value;
	SymbolType symbolType;
	if (!Context::resolveSymbol(_identifier, value, symbolType)) {
		Compiler::log(LOG_ERROR, "Could not resolve symbol \"%s\"", _identifier.c_str());
		return;
	}
	Common::String valueStr = Common::String::format("%u", value);
	if (symbolType == SYMBOL_VARIABLE)
		instructions.push_back(new Instruction("pushWordVar"));
	else
		instructions.push_back(new Instruction("pushWord"));
	instructions.push_back(new Instruction(VALUE_WORD, valueStr));
}

ConstantExpression::ConstantExpression(int32 number)
	: Expression(EXPRESSION_CONSTANT),
	  _number(number) {
}

void ConstantExpression::compile(Common::Array<Instruction *> &instructions) {
	Common::String valueStr = Common::String::format("%d", _number);
	instructions.push_back(new Instruction("pushWord"));
	instructions.push_back(new Instruction(VALUE_WORD, valueStr));
}

StringExpression::StringExpression(const Common::String &s)
	: Expression(EXPRESSION_STRING),
	  _string(s) {
}

Common::Array<byte> StringExpression::convertString(const Common::String &s) {
	Common::Array<byte> result;
	int pos = 0;

	while (pos < (int)s.size()) {
		switch (s[pos]) {
		case '\\': {
			Common::Array<byte> escaped = parseEscapeCharacter(s, ++pos);
			for (uint i = 0; i < escaped.size(); ++i)
				result.push_back(escaped[i]);
			break;
		}
		case '%': {
			Common::Array<byte> special = parseSpecialCode(s, ++pos);
			for (uint i = 0; i < special.size(); ++i)
				result.push_back(special[i]);
			break;
		}
		default:
			result.push_back((byte)s[pos++]);
		}
	}

	return result;
}

Common::Array<byte> StringExpression::parseEscapeCharacter(const Common::String &s, int &pos) {
	Common::Array<byte> result;
	uint32 value;

	// Get escape character
	switch (s[pos++]) {
	case '%':
		result.push_back('%');
		break;
	case 'n':
		result.push_back(STRING_OPCODE);
		result.push_back(1);
		break;
	case 'k':
		result.push_back(STRING_OPCODE);
		result.push_back(2);
		break;
	case 'w':
		result.push_back(STRING_OPCODE);
		result.push_back(3);
		break;
	case 'v':
		result.push_back(STRING_OPCODE);
		result.push_back(8);
		break;
	case 'x':
		if (sscanf(s.c_str() + pos, "%2x", &value) != 1) {
			Compiler::log(LOG_ERROR, "Error decoding hexadecimal number in string \"%s\"", s.c_str());
			return result;
		}
		result.push_back((byte)value);
		pos += 2;
		break;
	default:
		Compiler::log(LOG_ERROR,"Unknown escape character in string \"%s\"", s.c_str());
	}

	return result;
}

Common::Array<byte> StringExpression::parseSpecialCode(const Common::String &s, int &pos) {
	Common::Array<byte> result;

	// Special SCUMM string opcode
	result.push_back(STRING_OPCODE);

	// Get special code
	byte specialCode;
	switch (s[pos++]) {
	case 'i':
		specialCode = INT_MESSAGE;
		break;
	case 'v':
		specialCode = VERB_MESSAGE;
		break;
	case 'n':
		specialCode = NAME_MESSAGE;
		break;
	case 's':
		specialCode = STRING_MESSAGE;
		break;
	case 'V':
		specialCode = VOICE_MESSAGE;
		break;
	default:
		Compiler::log(LOG_ERROR,"Unknown special code in string \"%s\"", s.c_str());
		specialCode = 0;
	}
	result.push_back(specialCode);

	// Parse, resolve, and add symbol to the string
	Common::String symbol = parseSymbol(s, pos);
	uint32 value;
	if (sscanf(symbol.c_str(), "%u", &value) != 1) {
		SymbolType symbolType;
		if (!Context::resolveSymbol(symbol, value, symbolType))
			Compiler::log(LOG_ERROR,"Could not resolve symbol \"%s\" in string \"%s\"", symbol.c_str(), s.c_str());
	}
	result.push_back((byte)value);
	result.push_back((byte)(value >> 8));

	// Voice messages use %V{offset,length} and encode as 4 pairs of (0xFF 0x0A lo hi):
	// Pair 1 (already emitted above): low word of offset
	// Pair 2: high word of offset
	// Pair 3: low word of length
	// Pair 4: high word of length
	if (specialCode == VOICE_MESSAGE) {
		// The symbol contains "offset,length"; the offset was already parsed
		// into 'value' above, so skip it and extract the length
		uint32 length = 0;
		sscanf(symbol.c_str(), "%*u,%u", &length);

		result.push_back(STRING_OPCODE);
		result.push_back(VOICE_MESSAGE);
		result.push_back((byte)(value >> 16));
		result.push_back((byte)(value >> 24));

		result.push_back(STRING_OPCODE);
		result.push_back(VOICE_MESSAGE);
		result.push_back((byte)length);
		result.push_back((byte)(length >> 8));

		result.push_back(STRING_OPCODE);
		result.push_back(VOICE_MESSAGE);
		result.push_back((byte)(length >> 16));
		result.push_back((byte)(length >> 24));
	}

	return result;
}

Common::String StringExpression::parseSymbol(const Common::String &s, int &pos) {
	if (s[pos++] != '{') {
		Compiler::log(LOG_ERROR, "Expected '{' while parsing special code in string \"%s\"", s.c_str());
		return "";
	}

	// Get symbol
	Common::String symbol;
	for (;;) {
		if (pos >= (int)s.size()) {
			Compiler::log(LOG_ERROR, "Expected '}' while parsing special code in string \"%s\"", s.c_str());
			return symbol;
		}

		if (s[pos] == '}') {
			pos++;
			break;
		}

		symbol += s[pos++];
	}

	return symbol;
}

void StringExpression::compile(Common::Array<Instruction *> &instructions) {
	Compiler::log(LOG_ERROR,"String expressions can't be evaluated as is");
}

void StringExpression::assign(Common::Array<Instruction *> &instructions, uint32 address) {
	Common::String addrStr = Common::String::format("%u", address);

	// Push index
	instructions.push_back(new Instruction("pushByte"));
	instructions.push_back(new Instruction(VALUE_BYTE, "0"));

	// Set the address contents to 0 to avoid the arrayOps assignString
	// from nuking or writing to a non-existing array
	instructions.push_back(new Instruction("pushByte"));
	instructions.push_back(new Instruction(VALUE_BYTE, "0"));
	instructions.push_back(new Instruction("writeWordVar"));
	instructions.push_back(new Instruction(VALUE_WORD, addrStr));

	// Add arrayOps assignString instruction
	instructions.push_back(new Instruction("arrayOps"));
	instructions.push_back(new Instruction("assignString"));
	instructions.push_back(new Instruction(VALUE_WORD, addrStr));

	// Convert string and add the corresponding instruction
	instructions.push_back(new Instruction(convertString(_string), _string));
}

ListExpression::ListExpression()
	: Expression(EXPRESSION_LIST) {
}

void ListExpression::compile(Common::Array<Instruction *> &instructions) {
	Compiler::log(LOG_ERROR,"List expressions can't be evaluated as is");
}

void ListExpression::assign(Common::Array<Instruction *> &instructions, uint32 address) {
	// Push entries
	for (uint i = 0; i <_entries.size(); ++i)
		_entries[i]->compile(instructions);

	// Push dimensions
	Common::String sizeStr = Common::String::format("%u", _entries.size());
	instructions.push_back(new Instruction("pushWord"));
	instructions.push_back(new Instruction(VALUE_WORD, sizeStr));
	instructions.push_back(new Instruction("pushByte"));
	instructions.push_back(new Instruction(VALUE_BYTE, "0"));

	Common::String addrStr = Common::String::format("%u", address);

	// Set the address contents to 0 to avoid the arrayOps assignIntList
	// from nuking and writing to a non-existing array
	instructions.push_back(new Instruction("pushByte"));
	instructions.push_back(new Instruction(VALUE_BYTE, "0"));
	instructions.push_back(new Instruction("writeWordVar"));
	instructions.push_back(new Instruction(VALUE_WORD, addrStr));

	// Add arrayOps assignIntList instruction
	instructions.push_back(new Instruction("arrayOps"));
	instructions.push_back(new Instruction("assignIntList"));
	instructions.push_back(new Instruction(VALUE_WORD, addrStr));
}

ListExpression::~ListExpression() {
	for (uint i = 0; i <_entries.size(); ++i)
		delete _entries[i];
}

ListEntryExpression::ListEntryExpression(const Common::String &identifier, Expression *e)
	: AssignableExpression(EXPRESSION_LIST_ENTRY, identifier),
	  _e(e) {
}

void ListEntryExpression::compile(Common::Array<Instruction *> &instructions) {
	uint32 value;
	SymbolType symbolType;
	if (!Context::resolveSymbol(_identifier, value, symbolType)) {
		Compiler::log(LOG_ERROR, "Could not resolve symbol \"%s\"", _identifier.c_str());
		return;
	}
	Common::String valueStr = Common::String::format("%u", value);

	// Push base
	_e->compile(instructions);

	// wordArrayRead instruction
	instructions.push_back(new Instruction("wordArrayRead"));
	instructions.push_back(new Instruction(VALUE_WORD, valueStr));
}

ListEntryExpression::~ListEntryExpression() {
	delete _e;
}

UnaryExpression::UnaryExpression(ExpressionType type, Expression *e)
	: Expression(type),
	  _e(e) {
}

void UnaryExpression::compile(Common::Array<Instruction *> &instructions) {
	switch (_type) {
	case EXPRESSION_UMINUS:
		instructions.push_back(new Instruction("pushByte"));
		instructions.push_back(new Instruction(VALUE_BYTE, "0"));
		_e->compile(instructions);
		instructions.push_back(new Instruction("sub"));
		break;
	case EXPRESSION_NOT:
		_e->compile(instructions);
		instructions.push_back(new Instruction("not"));
		break;
	default:
		break;
	}
}

UnaryExpression::~UnaryExpression() {
	delete _e;
}

BinaryExpression::BinaryExpression(ExpressionType type, Expression *e1, Expression *e2)
	: Expression(type),
	  _e1(e1),
	  _e2(e2) {
}

void BinaryExpression::compile(Common::Array<Instruction *> &instructions) {
	_e1->compile(instructions);
	_e2->compile(instructions);

	switch (_type) {
	case EXPRESSION_ADD:
		instructions.push_back(new Instruction("add"));
		break;
	case EXPRESSION_SUB:
		instructions.push_back(new Instruction("sub"));
		break;
	case EXPRESSION_DIV:
		instructions.push_back(new Instruction("div"));
		break;
	case EXPRESSION_MUL:
		instructions.push_back(new Instruction("mul"));
		break;
	case EXPRESSION_LT:
		instructions.push_back(new Instruction("lt"));
		break;
	case EXPRESSION_GT:
		instructions.push_back(new Instruction("gt"));
		break;
	case EXPRESSION_BAND:
		instructions.push_back(new Instruction("band"));
		break;
	case EXPRESSION_BOR:
		instructions.push_back(new Instruction("bor"));
		break;
	case EXPRESSION_LE:
		instructions.push_back(new Instruction("le"));
		break;
	case EXPRESSION_GE:
		instructions.push_back(new Instruction("ge"));
		break;
	case EXPRESSION_NE:
		instructions.push_back(new Instruction("neq"));
		break;
	case EXPRESSION_EQ:
		instructions.push_back(new Instruction("eq"));
		break;
	case EXPRESSION_LAND:
		instructions.push_back(new Instruction("land"));
		break;
	case EXPRESSION_LOR:
		instructions.push_back(new Instruction("lor"));
		break;
	default:
		break;
	}
}

BinaryExpression::~BinaryExpression() {
	delete _e1;
	delete _e2;
}

AssignmentExpression::AssignmentExpression(AssignmentType assignmentType, bool preOperation, AssignableExpression *a, Expression *e)
	: Expression(EXPRESSION_ASSIGNMENT),
	  _assignmentType(assignmentType),
	  _preOperation(preOperation),
	  _assignableExpression(a),
	  _expression(e) {
}

void AssignmentExpression::compile(Common::Array<Instruction *> &instructions) {
	// Check the assignable expression type as it should be a variable or a list entry
	Common::String identifier = _assignableExpression->getIdentifier();
	uint32 address;
	SymbolType symbolType;
	if (!Context::resolveSymbol(identifier, address, symbolType)) {
		Compiler::log(LOG_ERROR, "Could not resolve symbol \"%s\"", identifier.c_str());
		return;
	}
	if (symbolType != SYMBOL_VARIABLE) {
		Compiler::log(LOG_ERROR, "Assignments only apply to variables and list entry expressions");
		return;
	}
	Common::String addrStr = Common::String::format("%u", address);

	// Push the assignable expression on the stack in case of a "post-operation"
	if (!_preOperation)
		_assignableExpression->compile(instructions);

	if (_assignableExpression->getType() == EXPRESSION_VARIABLE) {
		// We have to consider strings and arrays differently
		if (_expression->getType() == EXPRESSION_STRING) {
			if (_assignmentType != ASSIGNMENT_EQUAL) {
				Compiler::log(LOG_ERROR, "Incrementations and decrementations don't apply to strings");
				return;
			}
			((StringExpression *)_expression)->assign(instructions, address);
		} else if (_expression->getType() == EXPRESSION_LIST) {
			if (_assignmentType != ASSIGNMENT_EQUAL) {
				Compiler::log(LOG_ERROR, "Incrementations and decrementations don't apply to lists");
				return;
			}
			((ListExpression *)_expression)->assign(instructions, address);
		} else {
			switch (_assignmentType) {
			case ASSIGNMENT_EQUAL:
				_expression->compile(instructions);
				break;
			case ASSIGNMENT_INC:
				_assignableExpression->compile(instructions);
				_expression->compile(instructions);
				instructions.push_back(new Instruction("add"));
				break;
			case ASSIGNMENT_DEC:
				_assignableExpression->compile(instructions);
				_expression->compile(instructions);
				instructions.push_back(new Instruction("sub"));
				break;
			}

			// Write variable
			instructions.push_back(new Instruction("writeWordVar"));
			instructions.push_back(new Instruction(VALUE_WORD, addrStr));
		}
	}
	// It's a list entry expression
	else {
		if ((_expression->getType() == EXPRESSION_STRING) || (_expression->getType() == EXPRESSION_LIST)) {
			Compiler::log(LOG_ERROR, "Cannot assign strings or lists to list entry expressions");
			return;
		}

		// Push base
		((ListEntryExpression *)_assignableExpression)->getExpression()->compile(instructions);

		switch (_assignmentType) {
		case ASSIGNMENT_EQUAL:
			_expression->compile(instructions);
			break;
		case ASSIGNMENT_INC:
			_assignableExpression->compile(instructions);
			_expression->compile(instructions);
			instructions.push_back(new Instruction("add"));
			break;
		case ASSIGNMENT_DEC:
			_assignableExpression->compile(instructions);
			_expression->compile(instructions);
			instructions.push_back(new Instruction("sub"));
			break;
		}

		// wordArrayWrite instruction
		instructions.push_back(new Instruction("wordArrayWrite"));
		instructions.push_back(new Instruction(VALUE_WORD, addrStr));
	}

	// Push what we just wrote on the stack in case of a "pre-operation"
	if (_preOperation)
		_assignableExpression->compile(instructions);
}

AssignmentExpression::~AssignmentExpression() {
	delete _assignableExpression;
	delete _expression;
}

CallExpression::CallExpression(const Common::String &functionName)
	: Expression(EXPRESSION_CALL),
	  _functionName(functionName) {
}

void CallExpression::compile(Common::Array<Instruction *> &instructions) {
	// Get function
	Function *function = Context::getFunction(_functionName);
	if (function == nullptr) {
		Compiler::log(LOG_ERROR,"Function \"%s\" has not been declared", _functionName.c_str());
		return;
	}

	if (_parameters.size() != function->getNumberOfArguments())
		Compiler::log(LOG_ERROR,"Function \"%s\" should have %u argument(s)", _functionName.c_str(), function->getNumberOfArguments());

	// If the function is inlined, we compile it and add the instructions to the current function
	if (function->getType() == FUNCTION_INLINED) {
		// We first build a declarations list
		Common::Array<Declaration *> declarations;
		for (int i = 0; i < function->getNumberOfArguments(); ++i)
			declarations.push_back(new Declaration(DECLARATION_VAR, function->getArgument(i)->getName()));
		for (uint32 i = 0; i < function->getBlockStatement()->getNumberOfDeclarations(); ++i) {
			Declaration *declaration = function->getBlockStatement()->getDeclaration(i);
			// Variable and constant declaration have to be treated separately
			if (declaration->getType() == DECLARATION_VAR)
				declarations.push_back(new Declaration(declaration->getType(), declaration->getName()));
			else
				declarations.push_back(new Declaration(declaration->getType(), declaration->getName(), declaration->getValue()));
		}
		declarations.push_back(new Declaration(DECLARATION_VAR, "returnValue"));

		// Prepare labels first
		uint32 labelCounter = Context::labelCounter;
		Context::labelCounter++;

		// Create the inlined context and push it a first time to get the argument addresses
		Context *context = new Context(CONTEXT_INLINED, &declarations, nullptr, -1, -1, labelCounter);
		Context::pushContext(context);

		// Store the addresses of the function arguments
		Common::Array<uint16> argumentAddresses;
		for (uint i = 0; i <_parameters.size(); ++i) {
			uint32 value;
			SymbolType st;
			Context::resolveSymbol(function->getArgument(i)->getName(), value, st);
			argumentAddresses.push_back(value);
		}

		// Pop the inlined context as the parameters shouldn't be aware of it
		Context::popContext();
		delete context;

		// Push parameters
		for (uint i = 0; i <_parameters.size(); ++i) {
			Common::String argAddrStr = Common::String::format("%u", argumentAddresses[i]);

			// Special case for strings
			if (_parameters[i]->getType() == EXPRESSION_STRING)
				((StringExpression *)_parameters[i])->assign(instructions, argumentAddresses[i]);
			else if (_parameters[i]->getType() == EXPRESSION_LIST)
				((ListExpression *)_parameters[i])->assign(instructions, argumentAddresses[i]);
			else {
				_parameters[i]->compile(instructions);

				// writeWordVar instruction
				instructions.push_back(new Instruction("writeWordVar"));
				instructions.push_back(new Instruction(VALUE_WORD, argAddrStr));
			}
		}

		// Recreate the context and push it again for the block statement
		context = new Context(CONTEXT_INLINED, &declarations, nullptr, -1, -1, labelCounter);
		Context::pushContext(context);

		// Compile the function block statements
		for (uint32 i = 0; i < function->getBlockStatement()->getNumberOfStatements(); ++i)
			function->getBlockStatement()->getStatement(i)->compile(instructions);

		// We have to nuke the previous allocated arrays
		for (uint i = 0; i <_parameters.size(); ++i)
			if (_parameters[i]->getType() == EXPRESSION_STRING || _parameters[i]->getType() == EXPRESSION_LIST) {
				Common::String argAddrStr = Common::String::format("%u", argumentAddresses[i]);

				// undim array
				instructions.push_back(new Instruction("dimArray"));
				instructions.push_back(new Instruction("undimArray"));
				instructions.push_back(new Instruction(VALUE_WORD, argAddrStr));
			}

		// Return label
		instructions.push_back(new Instruction(labelCounter));

		// Push return value
		uint32 value;
		SymbolType st;
		Context::resolveSymbol("returnValue", value, st);
		Common::String retValueStr = Common::String::format("%u", value);
		instructions.push_back(new Instruction("pushWordVar"));
		instructions.push_back(new Instruction(VALUE_WORD, retValueStr));

		// No need to use the inlined context anymore
		Context::popContext();
		delete context;

		// Delete the previous created declarations
		for (uint i = 0; i <declarations.size(); ++i)
			delete declarations[i];
	} else {
		// Push function ID
		Common::String functionID = Common::String::format("%u", function->getID());
		instructions.push_back(new Instruction("pushWord"));
		instructions.push_back(new Instruction(VALUE_WORD, functionID));

		// function parameters
		for (uint i = 0; i <_parameters.size(); ++i)
			_parameters[i]->compile(instructions);

		// pushByte parameters size
		Common::String paramSize = Common::String::format("%u", _parameters.size());
		instructions.push_back(new Instruction("pushByte"));
		instructions.push_back(new Instruction(VALUE_BYTE, paramSize));

		// startScriptQuick instruction
		instructions.push_back(new Instruction("startScriptQuick"));

		// If the function is not treated as a thread, we just wait until its execution is over
		if (function->getType() == FUNCTION_NORMAL) {
			// Prepare labels first
			uint32 labelCounter = Context::labelCounter;
			Context::labelCounter++;

			instructions.push_back(new Instruction(labelCounter));

			// We let the scheduler take care of other threads
			instructions.push_back(new Instruction("breakHere"));

			// Push function ID
			instructions.push_back(new Instruction("pushWord"));
			instructions.push_back(new Instruction(VALUE_WORD, functionID));

			// isScriptRunning instruction
			instructions.push_back(new Instruction("isScriptRunning"));

			// if instruction
			Common::String labelStr = Common::String::format("LABEL_%u", labelCounter);
			instructions.push_back(new Instruction("if"));
			instructions.push_back(new Instruction(VALUE_WORD, labelStr));
		}

		// For now, we just push 0 as a return value for normal functions and threads
		instructions.push_back(new Instruction("pushByte"));
		instructions.push_back(new Instruction(VALUE_BYTE, "0"));
	}
}

CallExpression::~CallExpression() {
	for (uint i = 0; i <_parameters.size(); ++i)
		delete _parameters[i];
}

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm
