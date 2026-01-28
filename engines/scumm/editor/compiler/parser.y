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

%code requires {
#define FORBIDDEN_SYMBOL_EXCEPTION_FILE
#define FORBIDDEN_SYMBOL_EXCEPTION_fprintf
#define FORBIDDEN_SYMBOL_EXCEPTION_fwrite
#define FORBIDDEN_SYMBOL_EXCEPTION_fread
#define FORBIDDEN_SYMBOL_EXCEPTION_stdin
#define FORBIDDEN_SYMBOL_EXCEPTION_stdout
#define FORBIDDEN_SYMBOL_EXCEPTION_stderr
#define FORBIDDEN_SYMBOL_EXCEPTION_exit
#define FORBIDDEN_SYMBOL_EXCEPTION_getc
#include "common/array.h"
namespace Scumm { namespace Editor { namespace Compiler {
class Declaration;
class Function;
}}}
}

%{
#include "scumm/editor/compiler/compiler.h"
#include "scumm/editor/compiler/declaration.h"
#include "scumm/editor/compiler/expression.h"
#include "scumm/editor/compiler/function.h"
#include "scumm/editor/compiler/statement.h"
using namespace Scumm::Editor::Compiler;

extern int scumm_yylineno;

int yylex(void);
void yyerror(Common::Array<Declaration *> &declarations, Common::Array<Function *> &functions, const char *s);

// Vectors collecting nodes during parsing so that in case of a parsing error, we can still free memory
Common::Array<Declaration *> declarationCollector;
Common::Array<Expression *> expressionCollector;
Common::Array<Statement *> statementCollector;
Common::Array<Common::Array<Declaration *>> declarationListCollector;
Common::Array<Common::Array<Expression *>> expressionListCollector;
Common::Array<Common::Array<Statement *>> statementListCollector;
Common::Array<Common::String> assemblyTokens;

void resetCollectors() {
	declarationCollector.clear();
	expressionCollector.clear();
	statementCollector.clear();
	declarationListCollector.clear();
	expressionListCollector.clear();
	statementListCollector.clear();
	assemblyTokens.clear();
}

// For more information in case of a parsing error
#define YYERROR_VERBOSE
%}

// Used types : a number, a string, or a generic pointer which needs casting
%union
{
#define STRING_LENGTH 512
    int number;
	char string[STRING_LENGTH];
};

// Tokens declaration
%token <string> T_STRING
%token T_CONST
%token T_VAR
%token T_ENUM
%token T_ACTOR
%token T_VERB
%token T_CLASS
%token T_FUNCTION
%token T_INLINE
%token T_THREAD
%token T_IF
%token T_ELSE
%token T_SWITCH
%token T_ACTION
%token T_CASE
%token T_DEFAULT
%token T_FOR
%token T_WHILE
%token T_DO
%token T_CONTINUE
%token T_BREAK
%token T_RETURN
%token T_CUTSCENE
%token T_TRY
%token T_CATCH
%token T_FINALLY
%token T_ASSEMBLY
%token <number> T_NUMBER
%token <string> T_IDENTIFIER

// Priority of operators
%left '='
%left T_INC T_DEC
%left T_LAND T_LOR
%left T_GE T_LE T_EQ T_NE '>' '<'
%left '+' '-'
%left '&' '|'
%left '*' '/'
%nonassoc T_UMINUS
%nonassoc T_NOT
%nonassoc T_UNI_INC T_UNI_DEC

// Inputs to yyparse
%parse-param {Common::Array<Scumm::Editor::Compiler::Declaration *> &declarations}
%parse-param {Common::Array<Scumm::Editor::Compiler::Function *> &functions}

// Use a unique prefix to avoid symbol collisions with other flex/bison parsers
%define api.prefix {scumm_yy}

// Bison output file directives
%defines "engines/scumm/editor/compiler/grammar.h"
%output "engines/scumm/editor/compiler/grammar.cpp"

// Expected shift/reduce conflicts (dangling else, etc.)
%expect 4

// Starting grammar rule
%start file

%%

arg:
	T_VAR T_IDENTIFIER
	{
		Declaration *declaration = new Declaration(DECLARATION_VAR, $2);
		declarationCollector.push_back(declaration);
	}
	;

args:
	args ',' arg
	{
		Declaration *declaration = declarationCollector.back();
		declarationListCollector.back().push_back(declaration);
		declarationCollector.pop_back();
	}
	| arg
	{
		Common::Array<Declaration *> v;
		Declaration *declaration = declarationCollector.back();
		v.push_back(declaration);
		declarationCollector.pop_back();
		declarationListCollector.push_back(v);
	}
	| /* nothing */
	{
		Common::Array<Declaration *> v;
		declarationListCollector.push_back(v);
	}
	;

assemblyTokens:
	assemblyTokens T_STRING
	{
		assemblyTokens.push_back($2);
	}
	| /* nothing */
	;

assignable:
	T_IDENTIFIER
	{
		Expression *expression = new VariableExpression($1);
		expressionCollector.push_back(expression);
	}
	| T_IDENTIFIER '[' expression ']'
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *listEntryExpression = new ListEntryExpression($1, expression);
		expressionCollector.push_back(listEntryExpression);
	}
	;

blockStatement:
	'{' declarations statements '}'
	{
		BlockStatement *blockStatement = new BlockStatement();
		for (int i = 0; i < (int)declarationListCollector.back().size(); i++)
			blockStatement->addDeclaration(declarationListCollector.back()[i]);
		declarationListCollector.pop_back();
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			blockStatement->addStatement(statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(blockStatement);
	}

callExpression:
	T_IDENTIFIER '(' parameters ')'
	{
		CallExpression *callExpression = new CallExpression($1);
		for (int i = 0; i < (int)expressionListCollector.back().size(); i++)
			callExpression->addParameter(expressionListCollector.back()[i]);
		expressionListCollector.pop_back();
		expressionCollector.push_back(callExpression);
	}
	;

caseStatement:
	T_CASE expression ':' statements
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		CaseStatement *caseStatement = new CaseStatement(expression);
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			caseStatement->addStatement(statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(caseStatement);
	}
	| T_DEFAULT ':' statements
	{
		CaseStatement *defaultStatement = new CaseStatement();
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			defaultStatement->addStatement(statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(defaultStatement);
	}
	;

caseStatements:
	caseStatements caseStatement
	{
		CaseStatement *caseStatement = (CaseStatement *)statementCollector.back();
		statementListCollector.back().push_back(caseStatement);
		statementCollector.pop_back();
	}
	| caseStatement
	{
		Common::Array<Statement *> v;
		CaseStatement *caseStatement = (CaseStatement *)statementCollector.back();
		v.push_back(caseStatement);
		statementCollector.pop_back();
		statementListCollector.push_back(v);
	}
	;

declaration:
	T_CONST T_IDENTIFIER '=' T_NUMBER ';'
	{
		Declaration *declaration = new Declaration(DECLARATION_CONST, $2, $4);
		declarationCollector.push_back(declaration);
	}
	| T_VAR T_IDENTIFIER '@' T_NUMBER ';'
	{
		Declaration *declaration = new Declaration(DECLARATION_VAR, $2, $4);
		declarationCollector.push_back(declaration);
	}
	| T_VAR T_IDENTIFIER ';'
	{
		Declaration *declaration = new Declaration(DECLARATION_VAR, $2);
		declarationCollector.push_back(declaration);
	}
	| T_ACTOR T_IDENTIFIER ';'
	{
		Declaration *declaration = new Declaration(DECLARATION_ACTOR, $2);
		declarationCollector.push_back(declaration);
	}
	| T_VERB T_IDENTIFIER ';'
	{
		Declaration *declaration = new Declaration(DECLARATION_VERB, $2);
		declarationCollector.push_back(declaration);
	}
	| T_CLASS T_IDENTIFIER ';'
	{
		Declaration *declaration = new Declaration(DECLARATION_CLASS, $2);
		declarationCollector.push_back(declaration);
	}
	;

declarations:
	declarations declaration
	{
		Declaration *declaration = declarationCollector.back();
		declarationListCollector.back().push_back(declaration);
		declarationCollector.pop_back();
	}
	| /* nothing */
	{
		Common::Array<Declaration *> v;
		declarationListCollector.push_back(v);
	}
	;

enum:
	T_ENUM '{' enums '}' ';'

enums:
	enums ',' T_IDENTIFIER
	{
		Declaration *declaration = new Declaration(DECLARATION_CONST, $3, declarationCollector.size());
		declarationCollector.push_back(declaration);
	}
	| T_IDENTIFIER
	{
		Declaration *declaration = new Declaration(DECLARATION_CONST, $1, declarationCollector.size());
		declarationCollector.push_back(declaration);
	}
	;

expression:
	assignable
	| T_NUMBER
	{
		Expression *expression = new ConstantExpression($1);
		expressionCollector.push_back(expression);
	}
	| T_STRING
	{
		Expression *expression = new StringExpression($1);
		expressionCollector.push_back(expression);
	}
	| list
	| assignable '=' expression
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_EQUAL, false, assignableExpression, expression);
		expressionCollector.push_back(assignmentExpression);
	}
	| '(' expression ')'
	| '-' expression %prec T_UMINUS
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		expression = Expression::simplifyUnaryExpression(EXPRESSION_UMINUS, expression);
		expressionCollector.push_back(expression);
	}
	| '!' expression %prec T_NOT
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		expression = Expression::simplifyUnaryExpression(EXPRESSION_NOT, expression);
		expressionCollector.push_back(expression);
	}
	| expression '+' expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_ADD, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression '-' expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_SUB, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression '/' expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_DIV, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression '*' expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_MUL, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression '<' expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_LT, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression '>' expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_GT, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression '&' expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_BAND, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression '|' expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_BOR, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression T_LE expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_LE, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression T_GE expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_GE, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression T_NE expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_NE, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression T_EQ expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_EQ, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression T_LAND expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_LAND, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| expression T_LOR expression
	{
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_LOR, expression1, expression2);
		expressionCollector.push_back(expression);
	}
	| assignable T_INC expression
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_INC, false, assignableExpression, expression);
		expressionCollector.push_back(assignmentExpression);
	}
	| assignable T_DEC expression
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_DEC, false, assignableExpression, expression);
		expressionCollector.push_back(assignmentExpression);
	}
	| T_UNI_INC assignable
	{
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_INC, true, assignableExpression, new ConstantExpression(1));
		expressionCollector.push_back(assignmentExpression);
	}
	| assignable T_UNI_INC
	{
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_INC, false, assignableExpression, new ConstantExpression(1));
		expressionCollector.push_back(assignmentExpression);
	}
	| T_UNI_DEC assignable
	{
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_DEC, true, assignableExpression, new ConstantExpression(1));
		expressionCollector.push_back(assignmentExpression);
	}
	| assignable T_UNI_DEC
	{
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_DEC, false, assignableExpression, new ConstantExpression(1));
		expressionCollector.push_back(assignmentExpression);
	}
	| callExpression
	;

file:
	file fileItem
	| /* nothing */
	;

fileItem:
	declaration
	{
		Declaration *declaration = declarationCollector.back();
		declarations.push_back(declaration);
		declarationCollector.pop_back();
	}
	| enum
	{
		while (!declarationCollector.empty()) {
			Declaration *declaration = declarationCollector.back();
			declarations.push_back(declaration);
			declarationCollector.pop_back();
		}
	}
	| function
	;

function:
	T_FUNCTION T_IDENTIFIER '(' args ')' blockStatement
	{
		BlockStatement *blockStatement = (BlockStatement *)statementCollector.back();
		statementCollector.pop_back();
		Function *function = new Function(FUNCTION_NORMAL, $2, blockStatement);
		for (int i = 0; i < (int)declarationListCollector.back().size(); i++)
			function->addArgument(declarationListCollector.back()[i]);
		declarationListCollector.pop_back();
		functions.push_back(function);
	}
	| T_INLINE T_FUNCTION T_IDENTIFIER '(' args ')' blockStatement
	{
		BlockStatement *blockStatement = (BlockStatement *)statementCollector.back();
		statementCollector.pop_back();
		Function *function = new Function(FUNCTION_INLINED, $3, blockStatement);
		for (int i = 0; i < (int)declarationListCollector.back().size(); i++)
			function->addArgument(declarationListCollector.back()[i]);
		declarationListCollector.pop_back();
		functions.push_back(function);
	}
	| T_THREAD T_IDENTIFIER '(' args ')' blockStatement
	{
		BlockStatement *blockStatement = (BlockStatement *)statementCollector.back();
		statementCollector.pop_back();
		Function *function = new Function(FUNCTION_THREAD, $2, blockStatement);
		for (int i = 0; i < (int)declarationListCollector.back().size(); i++)
			function->addArgument(declarationListCollector.back()[i]);
		declarationListCollector.pop_back();
		functions.push_back(function);
	}
	;

list:
	'[' listEntries ']'
	{
		ListExpression *listExpression = new ListExpression();
		for (int i = 0; i < (int)expressionListCollector.back().size(); i++)
			listExpression->addEntry(expressionListCollector.back()[i]);
		expressionListCollector.pop_back();
		expressionCollector.push_back(listExpression);
	}

listEntries:
	listEntries ',' expression
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		expressionListCollector.back().push_back(expression);
	}
	| expression
	{
		Common::Array<Expression *> v;
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		v.push_back(expression);
		expressionListCollector.push_back(v);
	}
	| /* nothing */
	{
		Common::Array<Expression *> v;
		expressionListCollector.push_back(v);
	}
	;

parameters:
	parameters ',' expression
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		expressionListCollector.back().push_back(expression);
	}
	| expression
	{
		Common::Array<Expression *> v;
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		v.push_back(expression);
		expressionListCollector.push_back(v);
	}
	| /* nothing */
	{
		Common::Array<Expression *> v;
		expressionListCollector.push_back(v);
	}
	;

statement:
	blockStatement
	| expression ';'
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		ExpressionStatement *expressionStatement = new ExpressionStatement(expression);
		statementCollector.push_back(expressionStatement);
	}
	| T_IF '(' expression ')' statement
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		IfElseStatement *ifStatement = new IfElseStatement(expression, statement, NULL);
		statementCollector.push_back(ifStatement);
	}
	| T_IF '(' expression ')' statement T_ELSE statement
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		Statement *elseStatement = statementCollector.back();
		statementCollector.pop_back();
		Statement *ifStatement = statementCollector.back();
		statementCollector.pop_back();
		IfElseStatement *ifElseStatement = new IfElseStatement(expression, ifStatement, elseStatement);
		statementCollector.push_back(ifElseStatement);
	}
	| T_SWITCH '(' expression ')' '{' caseStatements '}'
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		SwitchStatement *switchStatement = new SwitchStatement(expression);
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			switchStatement->addCaseStatement((CaseStatement *)statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(switchStatement);
	}
	| T_ACTION '{' caseStatements '}'
	{
		ActionStatement *actionStatement = new ActionStatement();
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			actionStatement->addCaseStatement((CaseStatement *)statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(actionStatement);
	}
	| T_FOR '(' expression ';' expression ';' expression ')' statement
	{
		Expression *increaseExpression = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *conditionExpression = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *initExpression = expressionCollector.back();
		expressionCollector.pop_back();
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		ForStatement *forStatement = new ForStatement(new ExpressionStatement(initExpression), conditionExpression, new ExpressionStatement(increaseExpression), statement);
		statementCollector.push_back(forStatement);
	}
	| T_WHILE '(' expression ')' statement
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		WhileStatement *whileStatement = new WhileStatement(expression, statement);
		statementCollector.push_back(whileStatement);
	}
	| T_DO statement T_WHILE '(' expression ')' ';'
	{
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		DoWhileStatement *doWhileStatement = new DoWhileStatement(statement, expression);
		statementCollector.push_back(doWhileStatement);
	}
	| T_CONTINUE ';'
	{
		ContinueStatement *continueStatement = new ContinueStatement();
		statementCollector.push_back(continueStatement);
	}
	| T_BREAK ';'
	{
		BreakStatement *breakStatement = new BreakStatement();
		statementCollector.push_back(breakStatement);
	}
	| T_RETURN expression ';'
	{
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		ReturnStatement *returnStatement = new ReturnStatement(expression);
		statementCollector.push_back(returnStatement);
	}
	| T_RETURN ';'
	{
		ReturnStatement *returnStatement = new ReturnStatement();
		statementCollector.push_back(returnStatement);
	}
	| T_CUTSCENE '(' parameters ')' statement
	{
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		CutsceneStatement *cutsceneStatement = new CutsceneStatement(statement);
		for (int i = 0; i < (int)expressionListCollector.back().size(); i++)
			cutsceneStatement->addParameter(expressionListCollector.back()[i]);
		expressionListCollector.pop_back();
		statementCollector.push_back(cutsceneStatement);
	}
	| T_TRY statement
	{
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		TryCatchFinallyStatement *tryStatement = new TryCatchFinallyStatement(statement, NULL, NULL);
		statementCollector.push_back(tryStatement);
	}
	| T_TRY statement T_CATCH statement
	{
		Statement *catchStatement = statementCollector.back();
		statementCollector.pop_back();
		Statement *tryStatement = statementCollector.back();
		statementCollector.pop_back();
		TryCatchFinallyStatement *tryCatchStatement = new TryCatchFinallyStatement(tryStatement, catchStatement, NULL);
		statementCollector.push_back(tryCatchStatement);
	}
	| T_TRY statement T_FINALLY statement
	{
		Statement *finallyStatement = statementCollector.back();
		statementCollector.pop_back();
		Statement *tryStatement = statementCollector.back();
		statementCollector.pop_back();
		TryCatchFinallyStatement *tryFinallyStatement = new TryCatchFinallyStatement(tryStatement, NULL, finallyStatement);
		statementCollector.push_back(tryFinallyStatement);
	}
	| T_TRY statement T_CATCH statement T_FINALLY statement
	{
		Statement *finallyStatement = statementCollector.back();
		statementCollector.pop_back();
		Statement *catchStatement = statementCollector.back();
		statementCollector.pop_back();
		Statement *tryStatement = statementCollector.back();
		statementCollector.pop_back();
		TryCatchFinallyStatement *tryCatchFinallyStatement = new TryCatchFinallyStatement(tryStatement, catchStatement, finallyStatement);
		statementCollector.push_back(tryCatchFinallyStatement);
	}
	| T_ASSEMBLY '{' assemblyTokens '}'
	{
		AssemblyStatement *assemblyStatement = new AssemblyStatement();
		for (int i = 0; i < (int)assemblyTokens.size(); i++)
			assemblyStatement->addToken(assemblyTokens[i]);
		assemblyTokens.clear();
		statementCollector.push_back(assemblyStatement);
	}
	;

statements:
	statements statement
	{
		Statement *statement = statementCollector.back();
		statementListCollector.back().push_back(statement);
		statementCollector.pop_back();
	}
	| /* nothing */
	{
		Common::Array<Statement *> v;
		statementListCollector.push_back(v);
	}
	;

%%

void yyerror(Common::Array<Declaration *> &declarations, Common::Array<Function *> &functions, const char *s) {
	// Clean nodes which would otherwise cause a memory leak
	for (int i = 0; i < (int)declarationCollector.size(); i++)
		delete declarationCollector[i];
	for (int i = 0; i < (int)expressionCollector.size(); i++)
		delete expressionCollector[i];
	for (int i = 0; i < (int)statementCollector.size(); i++)
		delete statementCollector[i];
	for (int i = 0; i < (int)declarationListCollector.size(); i++)
		for (int j = 0; j < (int)declarationListCollector[i].size(); j++)
			delete declarationListCollector[i][j];
	for (int i = 0; i < (int)expressionListCollector.size(); i++)
		for (int j = 0; j < (int)expressionListCollector[i].size(); j++)
			delete expressionListCollector[i][j];
	for (int i = 0; i < (int)statementListCollector.size(); i++)
		for (int j = 0; j < (int)statementListCollector[i].size(); j++)
			delete statementListCollector[i][j];

	Compiler::log(LOG_ERROR, "Line %d: %s", scumm_yylineno, s);
}
