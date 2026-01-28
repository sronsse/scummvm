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

#include "scumm/editor/compiler/statement.h"
#include "scumm/editor/compiler/compiler.h"
#include "scumm/editor/compiler/context.h"
#include "scumm/editor/compiler/declaration.h"
#include "scumm/editor/compiler/expression.h"
#include "scumm/editor/compiler/instruction.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

const byte ActionStatement::HEADER_SIZE = 8;

ExpressionStatement::ExpressionStatement(Expression *e)
	: _expression(e) {
}

void ExpressionStatement::compile(Common::Array<Instruction *> &instructions) {
	// expression
	_expression->compile(instructions);

	// pop instruction
	instructions.push_back(new Instruction("pop"));
}

ExpressionStatement::~ExpressionStatement() {
	delete _expression;
}

void BlockStatement::compile(Common::Array<Instruction *> &instructions) {
	Context context(CONTEXT_BLOCK, &_declarations, nullptr, -1, -1, -1);
	Context::pushContext(&context);

	// block instructions
	for (uint i = 0; i < _statements.size(); ++i)
		_statements[i]->compile(instructions);

	Context::popContext();
}

BlockStatement::~BlockStatement() {
	for (uint i = 0; i < _declarations.size(); ++i)
		delete _declarations[i];
	for (uint i = 0; i < _statements.size(); ++i)
		delete _statements[i];
}

IfElseStatement::IfElseStatement(Expression *e, Statement *ifS, Statement *elseS)
	: _expression(e),
	  _ifStatement(ifS),
	  _elseStatement(elseS) {
}

void IfElseStatement::compile(Common::Array<Instruction *> &instructions) {
	// Prepare labels first
	uint32 labelCounter = Context::labelCounter;
	Context::labelCounter += _elseStatement != nullptr ? 2 : 1;

	// if condition
	_expression->compile(instructions);

	// ifNot instruction
	Common::String labelStr = Common::String::format("LABEL_%u", labelCounter);
	instructions.push_back(new Instruction("ifNot"));
	instructions.push_back(new Instruction(VALUE_WORD, labelStr));

	// if statement
	_ifStatement->compile(instructions);

	// The following code is generated only if an else statement is present
	if (_elseStatement != nullptr) {
		// jump instruction
		Common::String elseLabelStr = Common::String::format("LABEL_%u", labelCounter + 1);
		instructions.push_back(new Instruction("jump"));
		instructions.push_back(new Instruction(VALUE_WORD, elseLabelStr));
	}

	// label
	instructions.push_back(new Instruction(labelCounter));

	// The following code is generated only if an else statement is present
	if (_elseStatement != nullptr) {
		// else statement
		_elseStatement->compile(instructions);

		// label
		instructions.push_back(new Instruction(labelCounter + 1));
	}
}

IfElseStatement::~IfElseStatement() {
	delete _expression;
	delete _ifStatement;
	delete _elseStatement;
}

CaseStatement::CaseStatement(Expression *e)
	: _expression(e) {
}

// This function is called for switch "case" statements translation
void CaseStatement::compile(Common::Array<Instruction *> &instructions) {
	// case instructions
	for (uint i = 0; i < _statements.size(); ++i)
		_statements[i]->compile(instructions);
}

CaseStatement::~CaseStatement() {
	delete _expression;
	for (uint i = 0; i < _statements.size(); ++i)
		delete _statements[i];
}

SwitchStatement::SwitchStatement(Expression *e)
	: _expression(e) {
}

void SwitchStatement::compile(Common::Array<Instruction *> &instructions) {
	// Prepare labels first
	uint32 labelCounter = Context::labelCounter;
	Context::labelCounter += _caseStatements.size() + 1;

	Context context(CONTEXT_SWITCH, nullptr, nullptr, -1, labelCounter + _caseStatements.size(), -1);
	Context::pushContext(&context);

	// We first consider all the case conditions
	CaseStatement *defaultStatement = nullptr;
	Expression *caseExpression;
	int defaultLabel = -1;
	// Use a simple array to track seen values (replaces std::set)
	Common::Array<int16> caseValues;
	for (uint i = 0; i < _caseStatements.size(); ++i) {
		caseExpression = _caseStatements[i]->getExpression();

		// Check if it's not the default statement
		if (caseExpression == nullptr) {
			if (defaultStatement != nullptr)
				Compiler::log(LOG_ERROR,"Only one default statement is allowed in switch statements");
			defaultStatement = _caseStatements[i];
			defaultLabel = labelCounter + i;
			continue;
		}

		// switch expression
		_expression->compile(instructions);

		// case expression
		caseExpression->compile(instructions);

		// We check that the case expression is constant
		uint32 value;
		if (caseExpression->getType() == EXPRESSION_VARIABLE) {
			Common::String identifier = ((VariableExpression *)caseExpression)->getIdentifier();
			SymbolType symbolType;
			if (!Context::resolveSymbol(identifier, value, symbolType))
				Compiler::log(LOG_ERROR,"Could not resolve symbol \"%s\"", identifier.c_str());
			if (symbolType != SYMBOL_CONSTANT)
				Compiler::log(LOG_ERROR,"Case expression \"%s\" is not constant", identifier.c_str());
		} else {
			if (caseExpression->getType() != EXPRESSION_CONSTANT)
				Compiler::log(LOG_ERROR,"Case expressions should always be constant");
			value = ((ConstantExpression *)caseExpression)->getNumber();
		}

		// We check that the case expression has not already been used
		bool duplicate = false;
		for (uint j = 0; j < caseValues.size(); j++) {
			if (caseValues[j] == (int16)value) {
				duplicate = true;
				break;
			}
		}
		if (duplicate)
			Compiler::log(LOG_ERROR,"Case value \"%d\" already used", (int)value);
		caseValues.push_back((int16)value);

		// if instruction
		instructions.push_back(new Instruction("eq"));

		// if instruction
		Common::String caseLabelStr = Common::String::format("LABEL_%u", labelCounter + i);
		instructions.push_back(new Instruction("if"));
		instructions.push_back(new Instruction(VALUE_WORD, caseLabelStr));
	}

	// default case
	if (defaultStatement != nullptr) {
		// jump instruction
		Common::String defLabelStr = Common::String::format("LABEL_%d", defaultLabel);
		instructions.push_back(new Instruction("jump"));
		instructions.push_back(new Instruction(VALUE_WORD, defLabelStr));
	} else {
		Compiler::log(LOG_WARNING,"Switch statement has no default case");

		// jump instruction
		Common::String endLabelStr = Common::String::format("LABEL_%u", (uint32)(labelCounter + _caseStatements.size()));
		instructions.push_back(new Instruction("jump"));
		instructions.push_back(new Instruction(VALUE_WORD, endLabelStr));
	}

	// Then we compile the case statements
	for (uint i = 0; i < _caseStatements.size(); ++i) {
		instructions.push_back(new Instruction(labelCounter + i));
		_caseStatements[i]->compile(instructions);
	}

	Context::popContext();

	// label
	instructions.push_back(new Instruction((uint32)(labelCounter + _caseStatements.size())));
}

SwitchStatement::~SwitchStatement() {
	delete _expression;
	for (uint i = 0; i < _caseStatements.size(); ++i)
		delete _caseStatements[i];
}

void ActionStatement::compile(Common::Array<Instruction *> &instructions) {
	// Prepare labels first
	uint32 labelCounter = Context::labelCounter;
	Context::labelCounter++;

	// Set the verb table index
	uint16 verbTableIndex = instructions.size();

	Context context(CONTEXT_VERB, nullptr, nullptr, -1, labelCounter, -1);
	Context::pushContext(&context);

	// We first consider all the case conditions
	Expression *caseExpression;
	Common::Array<int16> caseValues;
	for (uint i = 0; i < _caseStatements.size(); ++i) {
		caseExpression = _caseStatements[i]->getExpression();

		// Check if it's not the default statement
		if (caseExpression == nullptr) {
			Compiler::log(LOG_ERROR, "Default statements are not allowed in action statements");
			continue;
		}

		// We check that the case expression is constant
		uint32 value;
		if (caseExpression->getType() == EXPRESSION_VARIABLE) {
			Common::String identifier = ((VariableExpression *)caseExpression)->getIdentifier();
			SymbolType symbolType;
			if (!Context::resolveSymbol(identifier, value, symbolType))
				Compiler::log(LOG_ERROR,"Could not resolve symbol \"%s\"", identifier.c_str());
			if (symbolType != SYMBOL_CONSTANT)
				Compiler::log(LOG_ERROR,"Case expression \"%s\" is not constant", identifier.c_str());
		} else {
			if (caseExpression->getType() != EXPRESSION_CONSTANT)
				Compiler::log(LOG_ERROR,"Case expressions should always be constant");
			value = ((ConstantExpression *)caseExpression)->getNumber();
		}

		// We check that the case expression has not already been used
		bool duplicate = false;
		for (uint j = 0; j < caseValues.size(); j++) {
			if (caseValues[j] == (int16)value) {
				duplicate = true;
				break;
			}
		}
		if (duplicate)
			Compiler::log(LOG_ERROR,"Case value \"%d\" already used", (int)value);
		caseValues.push_back((int16)value);

		// Add verb table entry (the address is filled later)
		Common::String valueStr = Common::String::format("%u", value);
		instructions.push_back(new Instruction(VALUE_BYTE, valueStr));
		instructions.push_back(new Instruction(VALUE_WORD, "0"));
	}

	// End of table index
	instructions.push_back(new Instruction(VALUE_BYTE, "0"));

	// Then we compile the case statements
	for (uint i = 0; i < _caseStatements.size(); ++i) {
		// Replace the verb address with a correct value
		Common::String addrStr = Common::String::format("%u", Context::currentAddress + HEADER_SIZE);
		instructions[verbTableIndex + i * 2 + 1]->setValue(addrStr);
		_caseStatements[i]->compile(instructions);
	}

	Context::popContext();

	// label
	instructions.push_back(new Instruction(labelCounter));
}

ActionStatement::~ActionStatement() {
	for (uint i = 0; i < _caseStatements.size(); ++i)
		delete _caseStatements[i];
}

ForStatement::ForStatement(ExpressionStatement *initES, Expression *conditionE, ExpressionStatement *increaseES, Statement *s)
	: _initExpressionStatement(initES),
	  _conditionExpression(conditionE),
	  _increaseExpressionStatement(increaseES),
	  _statement(s) {
}

void ForStatement::compile(Common::Array<Instruction *> &instructions) {
	// Prepare labels first
	uint32 labelCounter = Context::labelCounter;
	Context::labelCounter += 3;

	// for initialization
	_initExpressionStatement->compile(instructions);

	// label
	instructions.push_back(new Instruction(labelCounter));

	// for condition
	_conditionExpression->compile(instructions);

	// ifNot instruction
	Common::String endLabelStr = Common::String::format("LABEL_%u", labelCounter + 2);
	instructions.push_back(new Instruction("ifNot"));
	instructions.push_back(new Instruction(VALUE_WORD, endLabelStr));

	Context context(CONTEXT_FOR, nullptr, nullptr, labelCounter + 1, labelCounter + 2, -1);
	Context::pushContext(&context);

	// for statement
	_statement->compile(instructions);

	Context::popContext();

	// label
	instructions.push_back(new Instruction(labelCounter + 1));

	// for increase
	_increaseExpressionStatement->compile(instructions);

	// jump instruction
	Common::String startLabelStr = Common::String::format("LABEL_%u", labelCounter);
	instructions.push_back(new Instruction("jump"));
	instructions.push_back(new Instruction(VALUE_WORD, startLabelStr));

	// label
	instructions.push_back(new Instruction(labelCounter + 2));
}

ForStatement::~ForStatement() {
	delete _initExpressionStatement;
	delete _conditionExpression;
	delete _increaseExpressionStatement;
	delete _statement;
}

WhileStatement::WhileStatement(Expression *e, Statement *s)
	: _expression(e),
	  _statement(s) {
}

void WhileStatement::compile(Common::Array<Instruction *> &instructions) {
	// Prepare labels first
	uint32 labelCounter = Context::labelCounter;
	Context::labelCounter += 2;

	// label
	instructions.push_back(new Instruction(labelCounter));

	// while condition
	_expression->compile(instructions);

	// ifNot instruction
	Common::String endLabelStr = Common::String::format("LABEL_%u", labelCounter + 1);
	instructions.push_back(new Instruction("ifNot"));
	instructions.push_back(new Instruction(VALUE_WORD, endLabelStr));

	Context context(CONTEXT_WHILE, nullptr, nullptr, labelCounter, labelCounter + 1, -1);
	Context::pushContext(&context);

	// while statement
	_statement->compile(instructions);

	Context::popContext();

	// jump instruction
	Common::String startLabelStr = Common::String::format("LABEL_%u", labelCounter);
	instructions.push_back(new Instruction("jump"));
	instructions.push_back(new Instruction(VALUE_WORD, startLabelStr));

	// label
	instructions.push_back(new Instruction(labelCounter + 1));
}

WhileStatement::~WhileStatement() {
	delete _expression;
	delete _statement;
}

DoWhileStatement::DoWhileStatement(Statement *s, Expression *e)
	: _statement(s),
	  _expression(e) {
}

void DoWhileStatement::compile(Common::Array<Instruction *> &instructions) {
	// Prepare labels first
	uint32 labelCounter = Context::labelCounter;
	Context::labelCounter += 3;

	// label
	instructions.push_back(new Instruction(labelCounter));

	Context context(CONTEXT_DO_WHILE, nullptr, nullptr, labelCounter + 1, labelCounter + 2, -1);
	Context::pushContext(&context);

	// do statement
	_statement->compile(instructions);

	Context::popContext();

	// label
	instructions.push_back(new Instruction(labelCounter + 1));

	// while condition
	_expression->compile(instructions);

	// if instruction
	Common::String startLabelStr = Common::String::format("LABEL_%u", labelCounter);
	instructions.push_back(new Instruction("if"));
	instructions.push_back(new Instruction(VALUE_WORD, startLabelStr));

	// label
	instructions.push_back(new Instruction(labelCounter + 2));
}

DoWhileStatement::~DoWhileStatement() {
	delete _statement;
	delete _expression;
}

void ContinueStatement::compile(Common::Array<Instruction *> &instructions) {
	int32 label = Context::getContinueLabel();
	if (label == -1) {
		Compiler::log(LOG_ERROR, "Keyword \"continue\" can only be used within loops");
		return;
	}

	// jump instruction
	Common::String labelStr = Common::String::format("LABEL_%d", label);
	instructions.push_back(new Instruction("jump"));
	instructions.push_back(new Instruction(VALUE_WORD, labelStr));
}

void BreakStatement::compile(Common::Array<Instruction *> &instructions) {
	int32 label = Context::getBreakLabel();
	if (label == -1) {
		Compiler::log(LOG_ERROR, "Keyword \"break\" can only be used within loops or switches");
		return;
	}

	// jump instruction
	Common::String labelStr = Common::String::format("LABEL_%d", label);
	instructions.push_back(new Instruction("jump"));
	instructions.push_back(new Instruction(VALUE_WORD, labelStr));
}

ReturnStatement::ReturnStatement(Expression *e)
	: _expression(e) {
}

void ReturnStatement::compile(Common::Array<Instruction *> &instructions) {
	// If an expression has been specified, we return it (only if we're in an inlined function)
	if (_expression != nullptr && Context::getFunctionType() != FUNCTION_INLINED) {
		Compiler::log(LOG_ERROR, "Return values are only accepted in inlined functions");
		return;
	}

	if (Context::getFunctionType() == FUNCTION_INLINED && _expression != nullptr) {
		// Compile expression
		_expression->compile(instructions);

		// Write result
		uint32 value;
		SymbolType symbolType;
		Context::resolveSymbol("returnValue", value, symbolType);
		Common::String valueStr = Common::String::format("%u", value);
		instructions.push_back(new Instruction("writeWordVar"));
		instructions.push_back(new Instruction(VALUE_WORD, valueStr));
	}

	// jump instruction
	Common::String labelStr = Common::String::format("LABEL_%d", Context::getReturnLabel());
	instructions.push_back(new Instruction("jump"));
	instructions.push_back(new Instruction(VALUE_WORD, labelStr));
}

ReturnStatement::~ReturnStatement() {
	delete _expression;
}

CutsceneStatement::CutsceneStatement(Statement *s)
	: _statement(s) {
}

void CutsceneStatement::compile(Common::Array<Instruction *> &instructions) {
	// cutscene parameters
	for (uint i = 0; i < _parameters.size(); ++i)
		_parameters[i]->compile(instructions);

	// pushByte parameters size
	Common::String sizeStr = Common::String::format("%u", _parameters.size());
	instructions.push_back(new Instruction("pushByte"));
	instructions.push_back(new Instruction(VALUE_BYTE, sizeStr));

	// cutscene instruction
	instructions.push_back(new Instruction("cutscene"));

	// cutscene statement
	_statement->compile(instructions);

	// endCutscene instruction
	instructions.push_back(new Instruction("endCutscene"));
}

CutsceneStatement::~CutsceneStatement() {
	delete _statement;
	for (uint i = 0; i < _parameters.size(); ++i)
		delete _parameters[i];
}

TryCatchFinallyStatement::TryCatchFinallyStatement(Statement *tryS, Statement *catchS, Statement *finallyS)
	: _tryStatement(tryS),
	  _catchStatement(catchS),
	  _finallyStatement(finallyS) {
}

void TryCatchFinallyStatement::compile(Common::Array<Instruction *> &instructions) {
	// Prepare labels first
	uint32 labelCounter = Context::labelCounter;
	Context::labelCounter += _catchStatement != nullptr ? 2 : 1;

	// beginOverride instruction
	instructions.push_back(new Instruction("beginOverride"));

	// jump instruction
	Common::String labelStr = Common::String::format("LABEL_%u", labelCounter);
	instructions.push_back(new Instruction("jump"));
	instructions.push_back(new Instruction(VALUE_WORD, labelStr));

	// try statement
	_tryStatement->compile(instructions);

	// label
	instructions.push_back(new Instruction(labelCounter));

	// The following code is generated only if a catch statement is present
	if (_catchStatement != nullptr) {
		// Push VAR_OVERRIDE value
		uint32 value;
		SymbolType symbolType;
		Context::resolveSymbol("VAR_OVERRIDE", value, symbolType);
		Common::String valueStr = Common::String::format("%u", value);
		instructions.push_back(new Instruction("pushWordVar"));
		instructions.push_back(new Instruction(VALUE_WORD, valueStr));

		// ifNot instruction
		Common::String catchLabelStr = Common::String::format("LABEL_%u", labelCounter + 1);
		instructions.push_back(new Instruction("ifNot"));
		instructions.push_back(new Instruction(VALUE_WORD, catchLabelStr));

		// catch statement
		_catchStatement->compile(instructions);

		// label
		instructions.push_back(new Instruction(labelCounter + 1));
	}

	// finally statement (if present)
	if (_finallyStatement != nullptr)
		_finallyStatement->compile(instructions);

	// endOverride instruction
	instructions.push_back(new Instruction("endOverride"));
}

TryCatchFinallyStatement::~TryCatchFinallyStatement() {
	delete _tryStatement;
	delete _catchStatement;
	delete _finallyStatement;
}

void AssemblyStatement::compile(Common::Array<Instruction *> &instructions) {
	// Parse the block tokens
	uint32 i = 0;
	while (i < _tokens.size()) {
		Common::String token = _tokens[i++];

		// First, check for "value" instructions
		ValueType valueType = VALUE_NULL;
		if (token == ".byte")
			valueType = VALUE_BYTE;
		else if (token == ".word")
			valueType = VALUE_WORD;
		else if (token == ".string")
			valueType = VALUE_STRING;

		// Get next token if the instruction is in fact a value
		Common::String originalValue;
		if (valueType != VALUE_NULL) {
			if (i == _tokens.size()) {
				Compiler::log(LOG_ERROR, "A value is expected");
				return;
			}
			originalValue = _tokens[i++];
		}

		// Resolve symbols if necessary
		Common::String convertedValue;
		if (valueType == VALUE_BYTE || valueType == VALUE_WORD) {
			uint32 v;
			if (sscanf(originalValue.c_str(), "%u", &v) == 1) {
				convertedValue = originalValue;
			} else {
				SymbolType symbolType;
				if (!Context::resolveSymbol(originalValue, v, symbolType)) {
					Compiler::log(LOG_ERROR, "Could not resolve symbol \"%s\"", originalValue.c_str());
					return;
				}
				convertedValue = Common::String::format("%u", v);
			}
		} else if (valueType == VALUE_STRING) {
			Common::Array<byte> stringData = StringExpression::convertString(originalValue);
			instructions.push_back(new Instruction(stringData, originalValue));
			continue;
		}

		// Add instruction (which can be an opcode or a direct value)
		Instruction *instruction;
		if (valueType == VALUE_NULL)
			instruction = new Instruction(token);
		else
			instruction = new Instruction(valueType, convertedValue);

		instructions.push_back(instruction);
	}
}

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm
