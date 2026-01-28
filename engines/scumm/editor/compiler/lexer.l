%{
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

#define FORBIDDEN_SYMBOL_EXCEPTION_FILE
#define FORBIDDEN_SYMBOL_EXCEPTION_fprintf
#define FORBIDDEN_SYMBOL_EXCEPTION_fwrite
#define FORBIDDEN_SYMBOL_EXCEPTION_fread
#define FORBIDDEN_SYMBOL_EXCEPTION_stdin
#define FORBIDDEN_SYMBOL_EXCEPTION_stdout
#define FORBIDDEN_SYMBOL_EXCEPTION_stderr
#define FORBIDDEN_SYMBOL_EXCEPTION_exit
#define FORBIDDEN_SYMBOL_EXCEPTION_getc

#include "scumm/editor/compiler/compiler.h"
#include "scumm/editor/compiler/declaration.h"
#include "scumm/editor/compiler/expression.h"
#include "scumm/editor/compiler/function.h"
#include "scumm/editor/compiler/statement.h"
#include "scumm/editor/compiler/grammar.h"
using namespace Scumm::Editor::Compiler;

#define yylval scumm_yylval

int commentCaller;

static int parseString(char *buf, int maxLen);
%}

%option prefix="scumm_yy"
%option yylineno
%option nounput
%option noyywrap
%option never-interactive
%option nounistd
%option outfile="engines/scumm/editor/compiler/lexer.cpp"

%x SINGLE_LINE_COMMENT
%x MULTIPLE_LINES_COMMENT
%x ASSEMBLY

%%

 /* Common expressions */
"//"							{ commentCaller = INITIAL; BEGIN SINGLE_LINE_COMMENT; }
"/*"							{ commentCaller = INITIAL; BEGIN MULTIPLE_LINES_COMMENT; }
\"								{ parseString(yylval.string, STRING_LENGTH); return T_STRING; }
"const"							return T_CONST;
"var"							return T_VAR;
"enum"							return T_ENUM;
"actor"							return T_ACTOR;
"verb"							return T_VERB;
"class"							return T_CLASS;
"function"						return T_FUNCTION;
"inline"						return T_INLINE;
"thread"						return T_THREAD;
"if"							return T_IF;
"else"							return T_ELSE;
"switch"						return T_SWITCH;
"action"						return T_ACTION;
"case"							return T_CASE;
"default"						return T_DEFAULT;
"for"							return T_FOR;
"while"							return T_WHILE;
"do"							return T_DO;
"continue"						return T_CONTINUE;
"break"							return T_BREAK;
"return"						return T_RETURN;
"cutscene"						return T_CUTSCENE;
"try"							return T_TRY;
"catch"							return T_CATCH;
"finally"						return T_FINALLY;
"asm"							{ BEGIN ASSEMBLY; return T_ASSEMBLY; }
">="							return T_GE;
"<="							return T_LE;
"=="							return T_EQ;
"!="							return T_NE;
"&&"							return T_LAND;
"||"							return T_LOR;
"+="							return T_INC;
"-="							return T_DEC;
"++"							return T_UNI_INC;
"--"							return T_UNI_DEC;
[-+()<>=*/;{},!&|@:\[\]]		return *yytext;
[0-9]+							{ yylval.number = atoi(yytext); return T_NUMBER; }
"0x"[a-fA-F0-9]+				{ yylval.number = strtol(yytext, NULL, 16); return T_NUMBER; }
[a-zA-Z_]+[a-zA-Z0-9_]*			{ strncpy(yylval.string, yytext, STRING_LENGTH); yylval.string[STRING_LENGTH - 1] = 0; return T_IDENTIFIER; }
[ \n\t]|.						;

 /* Assembly expressions */
<ASSEMBLY>"//"				{ commentCaller = ASSEMBLY; BEGIN SINGLE_LINE_COMMENT; }
<ASSEMBLY>"/*"				{ commentCaller = ASSEMBLY; BEGIN MULTIPLE_LINES_COMMENT; }
<ASSEMBLY>"{"				return *yytext;
<ASSEMBLY>"}"				{ BEGIN INITIAL; return *yytext; }
<ASSEMBLY>\"				{ parseString(yylval.string, STRING_LENGTH); return T_STRING; }
<ASSEMBLY>[-+]*[0-9]+		{ Common::sprintf_s(yylval.string, "%d", atoi(yytext)); return T_STRING; }
<ASSEMBLY>"0x"[a-fA-F0-9]+	{ Common::sprintf_s(yylval.string, "%u", (unsigned int)strtol(yytext, NULL, 16)); return T_STRING; }
<ASSEMBLY>\'.\'				{ Common::sprintf_s(yylval.string, "%u", (unsigned int)yytext[1]); return T_STRING; }
<ASSEMBLY>\.*[a-zA-Z0-9_]+	{ strncpy(yylval.string, yytext, STRING_LENGTH); yylval.string[STRING_LENGTH - 1] = 0; return T_STRING; }
<ASSEMBLY>[ \n\t]|.			;

 /* Single line comments */
<SINGLE_LINE_COMMENT>\n			BEGIN commentCaller;
<SINGLE_LINE_COMMENT>.			;

 /* Multiple line comments */
<MULTIPLE_LINES_COMMENT>"*/"	BEGIN commentCaller;
<MULTIPLE_LINES_COMMENT>.|\n	;

%%

static int parseString(char *buf, int maxLen) {
	int lineNumber = yylineno;
	int i = 0;
	char c;
	char previous = 0;
	while ((i < maxLen - 1) && (c = yyinput())) {
		if (c == '"' && previous != '\\') {
			buf[i] = 0;
			return i;
		}
		buf[i++] = c;
		previous = c;
	}
	if (c != '"')
		Compiler::log(LOG_ERROR, "Line %u: string length too long !", lineNumber);
	buf[maxLen - 1] = 0;
	return i;
}
