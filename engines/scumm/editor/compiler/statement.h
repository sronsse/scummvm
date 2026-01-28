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

#ifndef SCUMM_EDITOR_COMPILER_STATEMENT_H
#define SCUMM_EDITOR_COMPILER_STATEMENT_H

#include "common/array.h"
#include "common/str.h"
#include "common/scummsys.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

class Declaration;
class Expression;
class VariableExpression;
class Instruction;

class Statement {
public:
	virtual void compile(Common::Array<Instruction *> &instructions) = 0;
	virtual ~Statement() {}
};

class ExpressionStatement : public Statement {
private:
	Expression *_expression;

public:
	ExpressionStatement(Expression *e);
	void compile(Common::Array<Instruction *> &instructions);
	~ExpressionStatement();
};

class BlockStatement : public Statement {
private:
	Common::Array<Declaration *> _declarations;
	Common::Array<Statement *> _statements;

public:
	void addDeclaration(Declaration *d) { _declarations.push_back(d); }
	uint32 getNumberOfDeclarations() { return _declarations.size(); }
	Declaration *getDeclaration(uint32 index) { return _declarations[index]; }
	void addStatement(Statement *s) { _statements.push_back(s); }
	uint32 getNumberOfStatements() { return _statements.size(); }
	Statement *getStatement(uint32 index) { return _statements[index]; }
	void compile(Common::Array<Instruction *> &instructions);
	~BlockStatement();
};

class IfElseStatement : public Statement {
private:
	Expression *_expression;
	Statement *_ifStatement;
	Statement *_elseStatement;

public:
	IfElseStatement(Expression *e, Statement *ifS, Statement *elseS);
	void compile(Common::Array<Instruction *> &instructions);
	~IfElseStatement();
};

class CaseStatement : public Statement {
private:
	Expression *_expression;
	Common::Array<Statement *> _statements;

public:
	CaseStatement(Expression *e = nullptr);
	Expression *getExpression() { return _expression; }
	void addStatement(Statement *s) { _statements.push_back(s); }
	void compile(Common::Array<Instruction *> &instructions);
	~CaseStatement();
};

class SwitchStatement : public Statement {
private:
	Expression *_expression;
	Common::Array<CaseStatement *> _caseStatements;

public:
	SwitchStatement(Expression *e);
	void addCaseStatement(CaseStatement *caseS) { _caseStatements.push_back(caseS); }
	void compile(Common::Array<Instruction *> &instructions);
	~SwitchStatement();
};

class ActionStatement : public Statement {
private:
	static const byte HEADER_SIZE;
	Common::Array<CaseStatement *> _caseStatements;

public:
	void addCaseStatement(CaseStatement *caseS) { _caseStatements.push_back(caseS); }
	void compile(Common::Array<Instruction *> &instructions);
	~ActionStatement();
};

class ForStatement : public Statement {
private:
	ExpressionStatement *_initExpressionStatement;
	Expression *_conditionExpression;
	ExpressionStatement *_increaseExpressionStatement;
	Statement *_statement;

public:
	ForStatement(ExpressionStatement *initES, Expression *conditionE, ExpressionStatement *increaseES, Statement *s);
	void compile(Common::Array<Instruction *> &instructions);
	~ForStatement();
};

class WhileStatement : public Statement {
private:
	Expression *_expression;
	Statement *_statement;

public:
	WhileStatement(Expression *e, Statement *s);
	void compile(Common::Array<Instruction *> &instructions);
	~WhileStatement();
};

class DoWhileStatement : public Statement {
private:
	Statement *_statement;
	Expression *_expression;

public:
	DoWhileStatement(Statement *s, Expression *e);
	void compile(Common::Array<Instruction *> &instructions);
	~DoWhileStatement();
};

class ContinueStatement : public Statement {
public:
	void compile(Common::Array<Instruction *> &instructions);
};

class BreakStatement : public Statement {
public:
	void compile(Common::Array<Instruction *> &instructions);
};

class ReturnStatement : public Statement {
	Expression *_expression;

public:
	ReturnStatement(Expression *e = nullptr);
	void compile(Common::Array<Instruction *> &instructions);
	~ReturnStatement();
};

class CutsceneStatement : public Statement {
private:
	Statement *_statement;
	Common::Array<Expression *> _parameters;

public:
	CutsceneStatement(Statement *s);
	void addParameter(Expression *parameter) { _parameters.push_back(parameter); }
	void compile(Common::Array<Instruction *> &instructions);
	~CutsceneStatement();
};

class TryCatchFinallyStatement : public Statement {
private:
	Statement *_tryStatement;
	Statement *_catchStatement;
	Statement *_finallyStatement;

public:
	TryCatchFinallyStatement(Statement *tryS, Statement *catchS, Statement *finallyS);
	void compile(Common::Array<Instruction *> &instructions);
	~TryCatchFinallyStatement();
};

class AssemblyStatement : public Statement {
private:
	Common::Array<Common::String> _tokens;

public:
	void addToken(const Common::String &token) { _tokens.push_back(token); }
	void compile(Common::Array<Instruction *> &instructions);
};

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm

#endif
