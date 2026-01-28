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

#ifndef SCUMM_EDITOR_COMPILER_EXPRESSION_H
#define SCUMM_EDITOR_COMPILER_EXPRESSION_H

#include "common/array.h"
#include "common/str.h"
#include "common/scummsys.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

class Instruction;

enum ExpressionType {
	EXPRESSION_VARIABLE,
	EXPRESSION_CONSTANT,
	EXPRESSION_STRING,
	EXPRESSION_LIST,
	EXPRESSION_LIST_ENTRY,
	EXPRESSION_UMINUS,
	EXPRESSION_NOT,
	EXPRESSION_ADD,
	EXPRESSION_SUB,
	EXPRESSION_DIV,
	EXPRESSION_MUL,
	EXPRESSION_LT,
	EXPRESSION_GT,
	EXPRESSION_BAND,
	EXPRESSION_BOR,
	EXPRESSION_LE,
	EXPRESSION_GE,
	EXPRESSION_NE,
	EXPRESSION_EQ,
	EXPRESSION_LAND,
	EXPRESSION_LOR,
	EXPRESSION_ASSIGNMENT,
	EXPRESSION_CALL
};

class Expression {
protected:
	ExpressionType _type;

public:
	static Expression *simplifyUnaryExpression(ExpressionType type, Expression *e);
	static Expression *simplifyBinaryExpression(ExpressionType type, Expression *e1, Expression *e2);

	Expression(ExpressionType type) : _type(type) {}
	ExpressionType getType() { return _type; }
	virtual void compile(Common::Array<Instruction *> &instructions) = 0;
	virtual ~Expression() {}
};

class AssignableExpression : public Expression {
protected:
	Common::String _identifier;

public:
	AssignableExpression(ExpressionType type, const Common::String &identifier) : Expression(type), _identifier(identifier) {}
	Common::String getIdentifier() { return _identifier; }
	virtual void compile(Common::Array<Instruction *> &instructions) = 0;
	virtual ~AssignableExpression() {}
};

class VariableExpression : public AssignableExpression {
public:
	VariableExpression(const Common::String &identifier);
	void compile(Common::Array<Instruction *> &instructions);
};

class ConstantExpression : public Expression {
private:
	int32 _number;

public:
	ConstantExpression(int32 number);
	int32 getNumber() { return _number; }
	void compile(Common::Array<Instruction *> &instructions);
};

class StringExpression : public Expression {
private:
	static const byte STRING_OPCODE;
	static const byte INT_MESSAGE;
	static const byte VERB_MESSAGE;
	static const byte NAME_MESSAGE;
	static const byte STRING_MESSAGE;
	static const byte VOICE_MESSAGE;

	Common::String _string;

	static Common::Array<byte> parseEscapeCharacter(const Common::String &s, int &pos);
	static Common::Array<byte> parseSpecialCode(const Common::String &s, int &pos);
	static Common::String parseSymbol(const Common::String &s, int &pos);

public:
	static Common::Array<byte> convertString(const Common::String &s);

	StringExpression(const Common::String &s);
	Common::String getString() { return _string; }
	void compile(Common::Array<Instruction *> &instructions);
	void assign(Common::Array<Instruction *> &instructions, uint32 address);
};

class ListExpression : public Expression {
private:
	Common::Array<Expression *> _entries;

public:
	ListExpression();
	void addEntry(Expression *entry) { _entries.push_back(entry); }
	void compile(Common::Array<Instruction *> &instructions);
	void assign(Common::Array<Instruction *> &instructions, uint32 address);
	~ListExpression();
};

class ListEntryExpression : public AssignableExpression {
private:
	Expression *_e;

public:
	ListEntryExpression(const Common::String &identifier, Expression *e);
	Expression *getExpression() { return _e; }
	void compile(Common::Array<Instruction *> &instructions);
	~ListEntryExpression();
};

class UnaryExpression : public Expression {
private:
	Expression *_e;

public:
	UnaryExpression(ExpressionType type, Expression *e);
	void compile(Common::Array<Instruction *> &instructions);
	~UnaryExpression();
};

class BinaryExpression : public Expression {
private:
	Expression *_e1;
	Expression *_e2;

public:
	BinaryExpression(ExpressionType type, Expression *e1, Expression *e2);
	void compile(Common::Array<Instruction *> &instructions);
	~BinaryExpression();
};

enum AssignmentType {
	ASSIGNMENT_EQUAL,
	ASSIGNMENT_INC,
	ASSIGNMENT_DEC
};

class AssignmentExpression : public Expression {
private:
	AssignmentType _assignmentType;
	bool _preOperation;
	AssignableExpression *_assignableExpression;
	Expression *_expression;

public:
	AssignmentExpression(AssignmentType assignmentType, bool preOperation, AssignableExpression *a, Expression *e);
	void compile(Common::Array<Instruction *> &instructions);
	~AssignmentExpression();
};

class CallExpression : public Expression {
private:
	Common::String _functionName;
	Common::Array<Expression *> _parameters;

public:
	CallExpression(const Common::String &functionName);
	void addParameter(Expression *parameter) { _parameters.push_back(parameter); }
	void compile(Common::Array<Instruction *> &instructions);
	~CallExpression();
};

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm

#endif
