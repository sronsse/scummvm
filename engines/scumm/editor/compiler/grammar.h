/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_SCUMM_YY_ENGINES_SCUMM_EDITOR_COMPILER_GRAMMAR_H_INCLUDED
# define YY_SCUMM_YY_ENGINES_SCUMM_EDITOR_COMPILER_GRAMMAR_H_INCLUDED
/* Debug traces.  */
#ifndef SCUMM_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define SCUMM_YYDEBUG 1
#  else
#   define SCUMM_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define SCUMM_YYDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined SCUMM_YYDEBUG */
#if SCUMM_YYDEBUG
extern int scumm_yydebug;
#endif
/* "%code requires" blocks.  */
#line 22 "engines/scumm/editor/compiler/parser.y"

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

#line 74 "engines/scumm/editor/compiler/grammar.h"

/* Token kinds.  */
#ifndef SCUMM_YYTOKENTYPE
# define SCUMM_YYTOKENTYPE
  enum scumm_yytokentype
  {
    SCUMM_YYEMPTY = -2,
    SCUMM_YYEOF = 0,               /* "end of file"  */
    SCUMM_YYerror = 256,           /* error  */
    SCUMM_YYUNDEF = 257,           /* "invalid token"  */
    T_STRING = 258,                /* T_STRING  */
    T_CONST = 259,                 /* T_CONST  */
    T_VAR = 260,                   /* T_VAR  */
    T_ENUM = 261,                  /* T_ENUM  */
    T_ACTOR = 262,                 /* T_ACTOR  */
    T_VERB = 263,                  /* T_VERB  */
    T_CLASS = 264,                 /* T_CLASS  */
    T_FUNCTION = 265,              /* T_FUNCTION  */
    T_INLINE = 266,                /* T_INLINE  */
    T_THREAD = 267,                /* T_THREAD  */
    T_IF = 268,                    /* T_IF  */
    T_ELSE = 269,                  /* T_ELSE  */
    T_SWITCH = 270,                /* T_SWITCH  */
    T_ACTION = 271,                /* T_ACTION  */
    T_CASE = 272,                  /* T_CASE  */
    T_DEFAULT = 273,               /* T_DEFAULT  */
    T_FOR = 274,                   /* T_FOR  */
    T_WHILE = 275,                 /* T_WHILE  */
    T_DO = 276,                    /* T_DO  */
    T_CONTINUE = 277,              /* T_CONTINUE  */
    T_BREAK = 278,                 /* T_BREAK  */
    T_RETURN = 279,                /* T_RETURN  */
    T_CUTSCENE = 280,              /* T_CUTSCENE  */
    T_TRY = 281,                   /* T_TRY  */
    T_CATCH = 282,                 /* T_CATCH  */
    T_FINALLY = 283,               /* T_FINALLY  */
    T_ASSEMBLY = 284,              /* T_ASSEMBLY  */
    T_NUMBER = 285,                /* T_NUMBER  */
    T_IDENTIFIER = 286,            /* T_IDENTIFIER  */
    T_INC = 287,                   /* T_INC  */
    T_DEC = 288,                   /* T_DEC  */
    T_LAND = 289,                  /* T_LAND  */
    T_LOR = 290,                   /* T_LOR  */
    T_GE = 291,                    /* T_GE  */
    T_LE = 292,                    /* T_LE  */
    T_EQ = 293,                    /* T_EQ  */
    T_NE = 294,                    /* T_NE  */
    T_UMINUS = 295,                /* T_UMINUS  */
    T_NOT = 296,                   /* T_NOT  */
    T_UNI_INC = 297,               /* T_UNI_INC  */
    T_UNI_DEC = 298                /* T_UNI_DEC  */
  };
  typedef enum scumm_yytokentype scumm_yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined SCUMM_YYSTYPE && ! defined SCUMM_YYSTYPE_IS_DECLARED
union SCUMM_YYSTYPE
{
#line 77 "engines/scumm/editor/compiler/parser.y"

#define STRING_LENGTH 512
    int number;
	char string[STRING_LENGTH];

#line 140 "engines/scumm/editor/compiler/grammar.h"

};
typedef union SCUMM_YYSTYPE SCUMM_YYSTYPE;
# define SCUMM_YYSTYPE_IS_TRIVIAL 1
# define SCUMM_YYSTYPE_IS_DECLARED 1
#endif


extern SCUMM_YYSTYPE scumm_yylval;


int scumm_yyparse (Common::Array<Scumm::Editor::Compiler::Declaration *> &declarations, Common::Array<Scumm::Editor::Compiler::Function *> &functions);


#endif /* !YY_SCUMM_YY_ENGINES_SCUMM_EDITOR_COMPILER_GRAMMAR_H_INCLUDED  */
