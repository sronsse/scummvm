/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         SCUMM_YYSTYPE
/* Substitute the variable and function names.  */
#define yyparse         scumm_yyparse
#define yylex           scumm_yylex
#define yyerror         scumm_yyerror
#define yydebug         scumm_yydebug
#define yynerrs         scumm_yynerrs
#define yylval          scumm_yylval
#define yychar          scumm_yychar

/* First part of user prologue.  */
#line 39 "engines/scumm/editor/compiler/parser.y"

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

#line 115 "engines/scumm/editor/compiler/grammar.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "grammar.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_STRING = 3,                   /* T_STRING  */
  YYSYMBOL_T_CONST = 4,                    /* T_CONST  */
  YYSYMBOL_T_VAR = 5,                      /* T_VAR  */
  YYSYMBOL_T_ENUM = 6,                     /* T_ENUM  */
  YYSYMBOL_T_ACTOR = 7,                    /* T_ACTOR  */
  YYSYMBOL_T_VERB = 8,                     /* T_VERB  */
  YYSYMBOL_T_CLASS = 9,                    /* T_CLASS  */
  YYSYMBOL_T_FUNCTION = 10,                /* T_FUNCTION  */
  YYSYMBOL_T_INLINE = 11,                  /* T_INLINE  */
  YYSYMBOL_T_THREAD = 12,                  /* T_THREAD  */
  YYSYMBOL_T_IF = 13,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 14,                    /* T_ELSE  */
  YYSYMBOL_T_SWITCH = 15,                  /* T_SWITCH  */
  YYSYMBOL_T_ACTION = 16,                  /* T_ACTION  */
  YYSYMBOL_T_CASE = 17,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 18,                 /* T_DEFAULT  */
  YYSYMBOL_T_FOR = 19,                     /* T_FOR  */
  YYSYMBOL_T_WHILE = 20,                   /* T_WHILE  */
  YYSYMBOL_T_DO = 21,                      /* T_DO  */
  YYSYMBOL_T_CONTINUE = 22,                /* T_CONTINUE  */
  YYSYMBOL_T_BREAK = 23,                   /* T_BREAK  */
  YYSYMBOL_T_RETURN = 24,                  /* T_RETURN  */
  YYSYMBOL_T_CUTSCENE = 25,                /* T_CUTSCENE  */
  YYSYMBOL_T_TRY = 26,                     /* T_TRY  */
  YYSYMBOL_T_CATCH = 27,                   /* T_CATCH  */
  YYSYMBOL_T_FINALLY = 28,                 /* T_FINALLY  */
  YYSYMBOL_T_ASSEMBLY = 29,                /* T_ASSEMBLY  */
  YYSYMBOL_T_NUMBER = 30,                  /* T_NUMBER  */
  YYSYMBOL_T_IDENTIFIER = 31,              /* T_IDENTIFIER  */
  YYSYMBOL_32_ = 32,                       /* '='  */
  YYSYMBOL_T_INC = 33,                     /* T_INC  */
  YYSYMBOL_T_DEC = 34,                     /* T_DEC  */
  YYSYMBOL_T_LAND = 35,                    /* T_LAND  */
  YYSYMBOL_T_LOR = 36,                     /* T_LOR  */
  YYSYMBOL_T_GE = 37,                      /* T_GE  */
  YYSYMBOL_T_LE = 38,                      /* T_LE  */
  YYSYMBOL_T_EQ = 39,                      /* T_EQ  */
  YYSYMBOL_T_NE = 40,                      /* T_NE  */
  YYSYMBOL_41_ = 41,                       /* '>'  */
  YYSYMBOL_42_ = 42,                       /* '<'  */
  YYSYMBOL_43_ = 43,                       /* '+'  */
  YYSYMBOL_44_ = 44,                       /* '-'  */
  YYSYMBOL_45_ = 45,                       /* '&'  */
  YYSYMBOL_46_ = 46,                       /* '|'  */
  YYSYMBOL_47_ = 47,                       /* '*'  */
  YYSYMBOL_48_ = 48,                       /* '/'  */
  YYSYMBOL_T_UMINUS = 49,                  /* T_UMINUS  */
  YYSYMBOL_T_NOT = 50,                     /* T_NOT  */
  YYSYMBOL_T_UNI_INC = 51,                 /* T_UNI_INC  */
  YYSYMBOL_T_UNI_DEC = 52,                 /* T_UNI_DEC  */
  YYSYMBOL_53_ = 53,                       /* ','  */
  YYSYMBOL_54_ = 54,                       /* '['  */
  YYSYMBOL_55_ = 55,                       /* ']'  */
  YYSYMBOL_56_ = 56,                       /* '{'  */
  YYSYMBOL_57_ = 57,                       /* '}'  */
  YYSYMBOL_58_ = 58,                       /* '('  */
  YYSYMBOL_59_ = 59,                       /* ')'  */
  YYSYMBOL_60_ = 60,                       /* ':'  */
  YYSYMBOL_61_ = 61,                       /* ';'  */
  YYSYMBOL_62_ = 62,                       /* '@'  */
  YYSYMBOL_63_ = 63,                       /* '!'  */
  YYSYMBOL_YYACCEPT = 64,                  /* $accept  */
  YYSYMBOL_arg = 65,                       /* arg  */
  YYSYMBOL_args = 66,                      /* args  */
  YYSYMBOL_assemblyTokens = 67,            /* assemblyTokens  */
  YYSYMBOL_assignable = 68,                /* assignable  */
  YYSYMBOL_blockStatement = 69,            /* blockStatement  */
  YYSYMBOL_callExpression = 70,            /* callExpression  */
  YYSYMBOL_caseStatement = 71,             /* caseStatement  */
  YYSYMBOL_caseStatements = 72,            /* caseStatements  */
  YYSYMBOL_declaration = 73,               /* declaration  */
  YYSYMBOL_declarations = 74,              /* declarations  */
  YYSYMBOL_enum = 75,                      /* enum  */
  YYSYMBOL_enums = 76,                     /* enums  */
  YYSYMBOL_expression = 77,                /* expression  */
  YYSYMBOL_file = 78,                      /* file  */
  YYSYMBOL_fileItem = 79,                  /* fileItem  */
  YYSYMBOL_function = 80,                  /* function  */
  YYSYMBOL_list = 81,                      /* list  */
  YYSYMBOL_listEntries = 82,               /* listEntries  */
  YYSYMBOL_parameters = 83,                /* parameters  */
  YYSYMBOL_statement = 84,                 /* statement  */
  YYSYMBOL_statements = 85                 /* statements  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined SCUMM_YYSTYPE_IS_TRIVIAL && SCUMM_YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   545

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  91
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  211

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   298


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    63,     2,     2,     2,     2,    45,     2,
      58,    59,    47,    43,    53,    44,     2,    48,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    60,    61,
      42,    32,    41,     2,    62,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    54,     2,    55,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,    46,    57,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    33,    34,    35,
      36,    37,    38,    39,    40,    49,    50,    51,    52
};

#if SCUMM_YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   146,   146,   154,   160,   169,   176,   180,   184,   189,
     199,   212,   223,   233,   244,   250,   261,   266,   271,   276,
     281,   286,   294,   301,   308,   311,   316,   324,   325,   330,
     335,   336,   345,   346,   353,   360,   369,   378,   387,   396,
     405,   414,   423,   432,   441,   450,   459,   468,   477,   486,
     495,   504,   511,   518,   525,   532,   536,   537,   541,   547,
     555,   559,   569,   579,   592,   602,   608,   617,   624,   630,
     639,   646,   647,   654,   663,   674,   684,   692,   705,   714,
     723,   728,   733,   740,   745,   755,   762,   771,   780,   791,
     802,   809
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if SCUMM_YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "T_STRING", "T_CONST",
  "T_VAR", "T_ENUM", "T_ACTOR", "T_VERB", "T_CLASS", "T_FUNCTION",
  "T_INLINE", "T_THREAD", "T_IF", "T_ELSE", "T_SWITCH", "T_ACTION",
  "T_CASE", "T_DEFAULT", "T_FOR", "T_WHILE", "T_DO", "T_CONTINUE",
  "T_BREAK", "T_RETURN", "T_CUTSCENE", "T_TRY", "T_CATCH", "T_FINALLY",
  "T_ASSEMBLY", "T_NUMBER", "T_IDENTIFIER", "'='", "T_INC", "T_DEC",
  "T_LAND", "T_LOR", "T_GE", "T_LE", "T_EQ", "T_NE", "'>'", "'<'", "'+'",
  "'-'", "'&'", "'|'", "'*'", "'/'", "T_UMINUS", "T_NOT", "T_UNI_INC",
  "T_UNI_DEC", "','", "'['", "']'", "'{'", "'}'", "'('", "')'", "':'",
  "';'", "'@'", "'!'", "$accept", "arg", "args", "assemblyTokens",
  "assignable", "blockStatement", "callExpression", "caseStatement",
  "caseStatements", "declaration", "declarations", "enum", "enums",
  "expression", "file", "fileItem", "function", "list", "listEntries",
  "parameters", "statement", "statements", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-165)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -165,   176,  -165,     9,    16,    30,    41,    65,    69,    72,
      74,    79,  -165,  -165,  -165,  -165,    84,   -40,    88,    59,
      86,    87,    91,   114,    92,   122,  -165,   125,  -165,   -34,
    -165,  -165,  -165,   152,   104,   152,   128,   129,   144,   134,
     160,  -165,   -39,   152,   -29,  -165,  -165,  -165,  -165,  -165,
     152,   140,   -22,   140,  -165,  -165,  -165,   140,  -165,   130,
    -165,  -165,   102,  -165,   139,   143,   142,   145,   147,   148,
     149,   151,    43,   150,   148,   153,  -165,    25,   163,   182,
     182,   163,  -165,   163,   163,     0,  -165,  -165,   192,  -165,
    -165,   163,   163,    27,   163,   163,   196,  -165,  -165,  -165,
     219,   163,    53,  -165,   163,   163,  -165,   164,  -165,  -165,
     497,   -44,   326,  -165,   163,   163,   163,  -165,  -165,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,  -165,   351,   376,   163,   159,  -165,   -16,
     246,   401,   162,  -165,   497,   -17,   148,   148,    10,   476,
      23,   163,  -165,  -165,   497,   497,   497,   204,   204,    45,
      45,    45,    45,    45,    45,    96,    96,    51,    51,  -165,
    -165,   148,   166,   300,  -165,  -165,  -165,   163,   148,   163,
     163,   148,   195,  -165,  -165,  -165,  -165,  -165,   497,   210,
      27,  -165,   148,   273,  -165,   426,   497,  -165,   148,   148,
     -14,   148,   163,   207,  -165,  -165,  -165,   451,  -165,   148,
    -165
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      57,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    58,    59,    56,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    18,     0,    26,     0,
      19,    20,    21,     5,     0,     5,     0,     0,     0,     0,
       0,     4,     0,     5,     0,    16,    17,    25,    24,     2,
       0,     0,     0,     0,     3,    23,    61,     0,    63,    91,
      62,    22,     0,    29,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    28,     8,     0,     0,
       0,    67,    10,     0,     0,    27,    71,    55,     0,    30,
      90,     0,     0,     0,     0,     0,     0,    80,    81,    83,
       0,    70,    85,     7,     0,    70,    33,     8,    51,    53,
      66,     0,     0,    34,     0,     0,     0,    52,    54,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    72,     0,     0,     0,     0,    15,     0,
       0,     0,     0,    82,    69,     0,     0,     0,     0,     0,
       0,     0,    64,    32,    31,    49,    50,    47,    48,    44,
      43,    46,    45,    40,    39,    35,    36,    41,    42,    38,
      37,     0,     0,     0,    91,    76,    14,     0,     0,     0,
       0,     0,    86,    87,     6,    89,     9,    11,    65,    73,
       0,    91,    13,     0,    78,     0,    68,    84,     0,     0,
       0,    12,     0,     0,    88,    74,    75,     0,    79,     0,
      77
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -165,   175,   -27,  -165,    28,    18,  -165,  -132,    80,   212,
    -165,  -165,  -165,   -66,  -165,  -165,  -165,  -165,  -165,   167,
     -69,  -164
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    41,    42,   148,    85,    86,    87,   138,   139,    12,
      59,    13,    29,    88,     1,    14,    15,    89,   111,   145,
      90,    62
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      96,   136,   137,   136,   137,   102,   100,   176,    44,   151,
     192,   152,   106,   184,    50,   110,    52,   112,   113,    38,
      51,    26,    27,    39,    50,   134,   135,   201,   140,   141,
      53,    50,   114,   115,   116,   144,   180,    57,   149,   144,
      16,   175,   181,   206,   136,   137,    63,    17,   154,   155,
     156,   117,   118,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   185,   176,    56,
     173,    58,    19,    76,    77,    60,   180,   182,   183,   104,
     146,   147,   187,   105,    23,   188,    18,    78,   127,   128,
     129,   130,   131,   132,    79,    80,    20,    81,   131,   132,
      21,    83,   189,    22,    99,    63,    84,   108,   109,   194,
      24,   193,   197,   195,   196,    64,    25,    65,    66,    28,
      30,    67,    68,    69,    70,    71,    72,    73,    74,   204,
     205,    75,    76,    77,     3,     4,   207,     6,     7,     8,
     210,   129,   130,   131,   132,    34,    78,    31,    32,    33,
      35,    63,    36,    79,    80,    37,    81,    40,    55,    82,
      83,    64,    43,    65,    66,    84,    63,    67,    68,    69,
      70,    71,    72,    73,    74,    47,     2,    75,    76,    77,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    45,
      46,    49,    78,    76,    77,    48,    55,    91,    93,    79,
      80,    92,    81,    94,    55,    95,    83,    78,   101,   103,
      97,    84,    98,   107,    79,    80,   142,    81,   104,   174,
     179,    83,   190,   198,   199,    54,    84,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   208,     0,
     200,    61,   150,     0,     0,     0,     0,     0,     0,     0,
     143,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   202,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     191,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   153,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     171,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   172,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     178,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   203,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,     0,     0,     0,     0,     0,
       0,   186,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132
};

static const yytype_int16 yycheck[] =
{
      69,    17,    18,    17,    18,    74,    72,   139,    35,    53,
     174,    55,    78,     3,    53,    81,    43,    83,    84,    53,
      59,    61,    62,    57,    53,    91,    92,   191,    94,    95,
      59,    53,    32,    33,    34,   101,    53,    59,   104,   105,
      31,    57,    59,    57,    17,    18,     3,    31,   114,   115,
     116,    51,    52,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,    57,   200,    51,
     136,    53,    31,    30,    31,    57,    53,   146,   147,    54,
      27,    28,    59,    58,    10,   151,    56,    44,    43,    44,
      45,    46,    47,    48,    51,    52,    31,    54,    47,    48,
      31,    58,   171,    31,    61,     3,    63,    79,    80,   178,
      31,   177,   181,   179,   180,    13,    32,    15,    16,    31,
      61,    19,    20,    21,    22,    23,    24,    25,    26,   198,
     199,    29,    30,    31,     4,     5,   202,     7,     8,     9,
     209,    45,    46,    47,    48,    31,    44,    61,    61,    58,
      58,     3,    30,    51,    52,    30,    54,     5,    56,    57,
      58,    13,    58,    15,    16,    63,     3,    19,    20,    21,
      22,    23,    24,    25,    26,    31,     0,    29,    30,    31,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    61,
      61,    31,    44,    30,    31,    61,    56,    58,    56,    51,
      52,    58,    54,    58,    56,    58,    58,    44,    58,    56,
      61,    63,    61,    31,    51,    52,    20,    54,    54,    60,
      58,    58,    56,    28,    14,    50,    63,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    61,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    61,    -1,
     190,    59,   105,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    78,     0,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    73,    75,    79,    80,    31,    31,    56,    31,
      31,    31,    31,    10,    31,    32,    61,    62,    31,    76,
      61,    61,    61,    58,    31,    58,    30,    30,    53,    57,
       5,    65,    66,    58,    66,    61,    61,    31,    61,    31,
      53,    59,    66,    59,    65,    56,    69,    59,    69,    74,
      69,    73,    85,     3,    13,    15,    16,    19,    20,    21,
      22,    23,    24,    25,    26,    29,    30,    31,    44,    51,
      52,    54,    57,    58,    63,    68,    69,    70,    77,    81,
      84,    58,    58,    56,    58,    58,    84,    61,    61,    61,
      77,    58,    84,    56,    54,    58,    77,    31,    68,    68,
      77,    82,    77,    77,    32,    33,    34,    51,    52,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    61,    77,    77,    17,    18,    71,    72,
      77,    77,    20,    61,    77,    83,    27,    28,    67,    77,
      83,    53,    55,    59,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    59,    59,    77,    60,    57,    71,    61,    59,    58,
      53,    59,    84,    84,     3,    57,    55,    59,    77,    84,
      56,    60,    85,    77,    84,    77,    77,    84,    28,    14,
      72,    85,    61,    59,    84,    84,    57,    77,    61,    59,
      84
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    64,    65,    66,    66,    66,    67,    67,    68,    68,
      69,    70,    71,    71,    72,    72,    73,    73,    73,    73,
      73,    73,    74,    74,    75,    76,    76,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    78,    78,    79,    79,
      79,    80,    80,    80,    81,    82,    82,    82,    83,    83,
      83,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      85,    85
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     3,     1,     0,     2,     0,     1,     4,
       4,     4,     4,     3,     2,     1,     5,     5,     3,     3,
       3,     3,     2,     0,     5,     3,     1,     1,     1,     1,
       1,     3,     3,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     2,     1,     2,     0,     1,     1,
       1,     6,     7,     6,     3,     3,     1,     0,     3,     1,
       0,     1,     2,     5,     7,     7,     4,     9,     5,     7,
       2,     2,     3,     2,     5,     2,     4,     4,     6,     4,
       2,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = SCUMM_YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == SCUMM_YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (declarations, functions, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use SCUMM_YYerror or SCUMM_YYUNDEF. */
#define YYERRCODE SCUMM_YYUNDEF


/* Enable debugging if requested.  */
#if SCUMM_YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, declarations, functions); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, Common::Array<Scumm::Editor::Compiler::Declaration *> &declarations, Common::Array<Scumm::Editor::Compiler::Function *> &functions)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (declarations);
  YY_USE (functions);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, Common::Array<Scumm::Editor::Compiler::Declaration *> &declarations, Common::Array<Scumm::Editor::Compiler::Function *> &functions)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, declarations, functions);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, Common::Array<Scumm::Editor::Compiler::Declaration *> &declarations, Common::Array<Scumm::Editor::Compiler::Function *> &functions)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], declarations, functions);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, declarations, functions); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !SCUMM_YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !SCUMM_YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, Common::Array<Scumm::Editor::Compiler::Declaration *> &declarations, Common::Array<Scumm::Editor::Compiler::Function *> &functions)
{
  YY_USE (yyvaluep);
  YY_USE (declarations);
  YY_USE (functions);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (Common::Array<Scumm::Editor::Compiler::Declaration *> &declarations, Common::Array<Scumm::Editor::Compiler::Function *> &functions)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = SCUMM_YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == SCUMM_YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= SCUMM_YYEOF)
    {
      yychar = SCUMM_YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == SCUMM_YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = SCUMM_YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = SCUMM_YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* arg: T_VAR T_IDENTIFIER  */
#line 147 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_VAR, (yyvsp[0].string));
		declarationCollector.push_back(declaration);
	}
#line 1398 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 3: /* args: args ',' arg  */
#line 155 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = declarationCollector.back();
		declarationListCollector.back().push_back(declaration);
		declarationCollector.pop_back();
	}
#line 1408 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 4: /* args: arg  */
#line 161 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Declaration *> v;
		Declaration *declaration = declarationCollector.back();
		v.push_back(declaration);
		declarationCollector.pop_back();
		declarationListCollector.push_back(v);
	}
#line 1420 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 5: /* args: %empty  */
#line 169 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Declaration *> v;
		declarationListCollector.push_back(v);
	}
#line 1429 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 6: /* assemblyTokens: assemblyTokens T_STRING  */
#line 177 "engines/scumm/editor/compiler/parser.y"
        {
		assemblyTokens.push_back((yyvsp[0].string));
	}
#line 1437 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 8: /* assignable: T_IDENTIFIER  */
#line 185 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = new VariableExpression((yyvsp[0].string));
		expressionCollector.push_back(expression);
	}
#line 1446 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 9: /* assignable: T_IDENTIFIER '[' expression ']'  */
#line 190 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *listEntryExpression = new ListEntryExpression((yyvsp[-3].string), expression);
		expressionCollector.push_back(listEntryExpression);
	}
#line 1457 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 10: /* blockStatement: '{' declarations statements '}'  */
#line 200 "engines/scumm/editor/compiler/parser.y"
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
#line 1472 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 11: /* callExpression: T_IDENTIFIER '(' parameters ')'  */
#line 213 "engines/scumm/editor/compiler/parser.y"
        {
		CallExpression *callExpression = new CallExpression((yyvsp[-3].string));
		for (int i = 0; i < (int)expressionListCollector.back().size(); i++)
			callExpression->addParameter(expressionListCollector.back()[i]);
		expressionListCollector.pop_back();
		expressionCollector.push_back(callExpression);
	}
#line 1484 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 12: /* caseStatement: T_CASE expression ':' statements  */
#line 224 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		CaseStatement *caseStatement = new CaseStatement(expression);
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			caseStatement->addStatement(statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(caseStatement);
	}
#line 1498 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 13: /* caseStatement: T_DEFAULT ':' statements  */
#line 234 "engines/scumm/editor/compiler/parser.y"
        {
		CaseStatement *defaultStatement = new CaseStatement();
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			defaultStatement->addStatement(statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(defaultStatement);
	}
#line 1510 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 14: /* caseStatements: caseStatements caseStatement  */
#line 245 "engines/scumm/editor/compiler/parser.y"
        {
		CaseStatement *caseStatement = (CaseStatement *)statementCollector.back();
		statementListCollector.back().push_back(caseStatement);
		statementCollector.pop_back();
	}
#line 1520 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 15: /* caseStatements: caseStatement  */
#line 251 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Statement *> v;
		CaseStatement *caseStatement = (CaseStatement *)statementCollector.back();
		v.push_back(caseStatement);
		statementCollector.pop_back();
		statementListCollector.push_back(v);
	}
#line 1532 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 16: /* declaration: T_CONST T_IDENTIFIER '=' T_NUMBER ';'  */
#line 262 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_CONST, (yyvsp[-3].string), (yyvsp[-1].number));
		declarationCollector.push_back(declaration);
	}
#line 1541 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 17: /* declaration: T_VAR T_IDENTIFIER '@' T_NUMBER ';'  */
#line 267 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_VAR, (yyvsp[-3].string), (yyvsp[-1].number));
		declarationCollector.push_back(declaration);
	}
#line 1550 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 18: /* declaration: T_VAR T_IDENTIFIER ';'  */
#line 272 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_VAR, (yyvsp[-1].string));
		declarationCollector.push_back(declaration);
	}
#line 1559 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 19: /* declaration: T_ACTOR T_IDENTIFIER ';'  */
#line 277 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_ACTOR, (yyvsp[-1].string));
		declarationCollector.push_back(declaration);
	}
#line 1568 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 20: /* declaration: T_VERB T_IDENTIFIER ';'  */
#line 282 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_VERB, (yyvsp[-1].string));
		declarationCollector.push_back(declaration);
	}
#line 1577 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 21: /* declaration: T_CLASS T_IDENTIFIER ';'  */
#line 287 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_CLASS, (yyvsp[-1].string));
		declarationCollector.push_back(declaration);
	}
#line 1586 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 22: /* declarations: declarations declaration  */
#line 295 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = declarationCollector.back();
		declarationListCollector.back().push_back(declaration);
		declarationCollector.pop_back();
	}
#line 1596 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 23: /* declarations: %empty  */
#line 301 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Declaration *> v;
		declarationListCollector.push_back(v);
	}
#line 1605 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 25: /* enums: enums ',' T_IDENTIFIER  */
#line 312 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_CONST, (yyvsp[0].string), declarationCollector.size());
		declarationCollector.push_back(declaration);
	}
#line 1614 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 26: /* enums: T_IDENTIFIER  */
#line 317 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = new Declaration(DECLARATION_CONST, (yyvsp[0].string), declarationCollector.size());
		declarationCollector.push_back(declaration);
	}
#line 1623 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 28: /* expression: T_NUMBER  */
#line 326 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = new ConstantExpression((yyvsp[0].number));
		expressionCollector.push_back(expression);
	}
#line 1632 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 29: /* expression: T_STRING  */
#line 331 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = new StringExpression((yyvsp[0].string));
		expressionCollector.push_back(expression);
	}
#line 1641 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 31: /* expression: assignable '=' expression  */
#line 337 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_EQUAL, false, assignableExpression, expression);
		expressionCollector.push_back(assignmentExpression);
	}
#line 1654 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 33: /* expression: '-' expression  */
#line 347 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		expression = Expression::simplifyUnaryExpression(EXPRESSION_UMINUS, expression);
		expressionCollector.push_back(expression);
	}
#line 1665 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 34: /* expression: '!' expression  */
#line 354 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		expression = Expression::simplifyUnaryExpression(EXPRESSION_NOT, expression);
		expressionCollector.push_back(expression);
	}
#line 1676 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 35: /* expression: expression '+' expression  */
#line 361 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_ADD, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1689 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 36: /* expression: expression '-' expression  */
#line 370 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_SUB, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1702 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 37: /* expression: expression '/' expression  */
#line 379 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_DIV, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1715 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 38: /* expression: expression '*' expression  */
#line 388 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_MUL, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1728 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 39: /* expression: expression '<' expression  */
#line 397 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_LT, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1741 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 40: /* expression: expression '>' expression  */
#line 406 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_GT, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1754 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 41: /* expression: expression '&' expression  */
#line 415 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_BAND, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1767 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 42: /* expression: expression '|' expression  */
#line 424 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_BOR, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1780 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 43: /* expression: expression T_LE expression  */
#line 433 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_LE, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1793 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 44: /* expression: expression T_GE expression  */
#line 442 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_GE, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1806 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 45: /* expression: expression T_NE expression  */
#line 451 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_NE, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1819 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 46: /* expression: expression T_EQ expression  */
#line 460 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_EQ, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1832 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 47: /* expression: expression T_LAND expression  */
#line 469 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_LAND, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1845 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 48: /* expression: expression T_LOR expression  */
#line 478 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression2 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression1 = expressionCollector.back();
		expressionCollector.pop_back();
		Expression *expression = Expression::simplifyBinaryExpression(EXPRESSION_LOR, expression1, expression2);
		expressionCollector.push_back(expression);
	}
#line 1858 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 49: /* expression: assignable T_INC expression  */
#line 487 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_INC, false, assignableExpression, expression);
		expressionCollector.push_back(assignmentExpression);
	}
#line 1871 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 50: /* expression: assignable T_DEC expression  */
#line 496 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_DEC, false, assignableExpression, expression);
		expressionCollector.push_back(assignmentExpression);
	}
#line 1884 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 51: /* expression: T_UNI_INC assignable  */
#line 505 "engines/scumm/editor/compiler/parser.y"
        {
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_INC, true, assignableExpression, new ConstantExpression(1));
		expressionCollector.push_back(assignmentExpression);
	}
#line 1895 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 52: /* expression: assignable T_UNI_INC  */
#line 512 "engines/scumm/editor/compiler/parser.y"
        {
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_INC, false, assignableExpression, new ConstantExpression(1));
		expressionCollector.push_back(assignmentExpression);
	}
#line 1906 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 53: /* expression: T_UNI_DEC assignable  */
#line 519 "engines/scumm/editor/compiler/parser.y"
        {
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_DEC, true, assignableExpression, new ConstantExpression(1));
		expressionCollector.push_back(assignmentExpression);
	}
#line 1917 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 54: /* expression: assignable T_UNI_DEC  */
#line 526 "engines/scumm/editor/compiler/parser.y"
        {
		AssignableExpression *assignableExpression = (AssignableExpression *)expressionCollector.back();
		expressionCollector.pop_back();
		AssignmentExpression *assignmentExpression = new AssignmentExpression(ASSIGNMENT_DEC, false, assignableExpression, new ConstantExpression(1));
		expressionCollector.push_back(assignmentExpression);
	}
#line 1928 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 58: /* fileItem: declaration  */
#line 542 "engines/scumm/editor/compiler/parser.y"
        {
		Declaration *declaration = declarationCollector.back();
		declarations.push_back(declaration);
		declarationCollector.pop_back();
	}
#line 1938 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 59: /* fileItem: enum  */
#line 548 "engines/scumm/editor/compiler/parser.y"
        {
		while (!declarationCollector.empty()) {
			Declaration *declaration = declarationCollector.back();
			declarations.push_back(declaration);
			declarationCollector.pop_back();
		}
	}
#line 1950 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 61: /* function: T_FUNCTION T_IDENTIFIER '(' args ')' blockStatement  */
#line 560 "engines/scumm/editor/compiler/parser.y"
        {
		BlockStatement *blockStatement = (BlockStatement *)statementCollector.back();
		statementCollector.pop_back();
		Function *function = new Function(FUNCTION_NORMAL, (yyvsp[-4].string), blockStatement);
		for (int i = 0; i < (int)declarationListCollector.back().size(); i++)
			function->addArgument(declarationListCollector.back()[i]);
		declarationListCollector.pop_back();
		functions.push_back(function);
	}
#line 1964 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 62: /* function: T_INLINE T_FUNCTION T_IDENTIFIER '(' args ')' blockStatement  */
#line 570 "engines/scumm/editor/compiler/parser.y"
        {
		BlockStatement *blockStatement = (BlockStatement *)statementCollector.back();
		statementCollector.pop_back();
		Function *function = new Function(FUNCTION_INLINED, (yyvsp[-4].string), blockStatement);
		for (int i = 0; i < (int)declarationListCollector.back().size(); i++)
			function->addArgument(declarationListCollector.back()[i]);
		declarationListCollector.pop_back();
		functions.push_back(function);
	}
#line 1978 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 63: /* function: T_THREAD T_IDENTIFIER '(' args ')' blockStatement  */
#line 580 "engines/scumm/editor/compiler/parser.y"
        {
		BlockStatement *blockStatement = (BlockStatement *)statementCollector.back();
		statementCollector.pop_back();
		Function *function = new Function(FUNCTION_THREAD, (yyvsp[-4].string), blockStatement);
		for (int i = 0; i < (int)declarationListCollector.back().size(); i++)
			function->addArgument(declarationListCollector.back()[i]);
		declarationListCollector.pop_back();
		functions.push_back(function);
	}
#line 1992 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 64: /* list: '[' listEntries ']'  */
#line 593 "engines/scumm/editor/compiler/parser.y"
        {
		ListExpression *listExpression = new ListExpression();
		for (int i = 0; i < (int)expressionListCollector.back().size(); i++)
			listExpression->addEntry(expressionListCollector.back()[i]);
		expressionListCollector.pop_back();
		expressionCollector.push_back(listExpression);
	}
#line 2004 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 65: /* listEntries: listEntries ',' expression  */
#line 603 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		expressionListCollector.back().push_back(expression);
	}
#line 2014 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 66: /* listEntries: expression  */
#line 609 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Expression *> v;
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		v.push_back(expression);
		expressionListCollector.push_back(v);
	}
#line 2026 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 67: /* listEntries: %empty  */
#line 617 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Expression *> v;
		expressionListCollector.push_back(v);
	}
#line 2035 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 68: /* parameters: parameters ',' expression  */
#line 625 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		expressionListCollector.back().push_back(expression);
	}
#line 2045 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 69: /* parameters: expression  */
#line 631 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Expression *> v;
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		v.push_back(expression);
		expressionListCollector.push_back(v);
	}
#line 2057 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 70: /* parameters: %empty  */
#line 639 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Expression *> v;
		expressionListCollector.push_back(v);
	}
#line 2066 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 72: /* statement: expression ';'  */
#line 648 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		ExpressionStatement *expressionStatement = new ExpressionStatement(expression);
		statementCollector.push_back(expressionStatement);
	}
#line 2077 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 73: /* statement: T_IF '(' expression ')' statement  */
#line 655 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		IfElseStatement *ifStatement = new IfElseStatement(expression, statement, NULL);
		statementCollector.push_back(ifStatement);
	}
#line 2090 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 74: /* statement: T_IF '(' expression ')' statement T_ELSE statement  */
#line 664 "engines/scumm/editor/compiler/parser.y"
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
#line 2105 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 75: /* statement: T_SWITCH '(' expression ')' '{' caseStatements '}'  */
#line 675 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		SwitchStatement *switchStatement = new SwitchStatement(expression);
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			switchStatement->addCaseStatement((CaseStatement *)statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(switchStatement);
	}
#line 2119 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 76: /* statement: T_ACTION '{' caseStatements '}'  */
#line 685 "engines/scumm/editor/compiler/parser.y"
        {
		ActionStatement *actionStatement = new ActionStatement();
		for (int i = 0; i < (int)statementListCollector.back().size(); i++)
			actionStatement->addCaseStatement((CaseStatement *)statementListCollector.back()[i]);
		statementListCollector.pop_back();
		statementCollector.push_back(actionStatement);
	}
#line 2131 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 77: /* statement: T_FOR '(' expression ';' expression ';' expression ')' statement  */
#line 693 "engines/scumm/editor/compiler/parser.y"
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
#line 2148 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 78: /* statement: T_WHILE '(' expression ')' statement  */
#line 706 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		WhileStatement *whileStatement = new WhileStatement(expression, statement);
		statementCollector.push_back(whileStatement);
	}
#line 2161 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 79: /* statement: T_DO statement T_WHILE '(' expression ')' ';'  */
#line 715 "engines/scumm/editor/compiler/parser.y"
        {
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		DoWhileStatement *doWhileStatement = new DoWhileStatement(statement, expression);
		statementCollector.push_back(doWhileStatement);
	}
#line 2174 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 80: /* statement: T_CONTINUE ';'  */
#line 724 "engines/scumm/editor/compiler/parser.y"
        {
		ContinueStatement *continueStatement = new ContinueStatement();
		statementCollector.push_back(continueStatement);
	}
#line 2183 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 81: /* statement: T_BREAK ';'  */
#line 729 "engines/scumm/editor/compiler/parser.y"
        {
		BreakStatement *breakStatement = new BreakStatement();
		statementCollector.push_back(breakStatement);
	}
#line 2192 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 82: /* statement: T_RETURN expression ';'  */
#line 734 "engines/scumm/editor/compiler/parser.y"
        {
		Expression *expression = expressionCollector.back();
		expressionCollector.pop_back();
		ReturnStatement *returnStatement = new ReturnStatement(expression);
		statementCollector.push_back(returnStatement);
	}
#line 2203 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 83: /* statement: T_RETURN ';'  */
#line 741 "engines/scumm/editor/compiler/parser.y"
        {
		ReturnStatement *returnStatement = new ReturnStatement();
		statementCollector.push_back(returnStatement);
	}
#line 2212 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 84: /* statement: T_CUTSCENE '(' parameters ')' statement  */
#line 746 "engines/scumm/editor/compiler/parser.y"
        {
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		CutsceneStatement *cutsceneStatement = new CutsceneStatement(statement);
		for (int i = 0; i < (int)expressionListCollector.back().size(); i++)
			cutsceneStatement->addParameter(expressionListCollector.back()[i]);
		expressionListCollector.pop_back();
		statementCollector.push_back(cutsceneStatement);
	}
#line 2226 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 85: /* statement: T_TRY statement  */
#line 756 "engines/scumm/editor/compiler/parser.y"
        {
		Statement *statement = statementCollector.back();
		statementCollector.pop_back();
		TryCatchFinallyStatement *tryStatement = new TryCatchFinallyStatement(statement, NULL, NULL);
		statementCollector.push_back(tryStatement);
	}
#line 2237 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 86: /* statement: T_TRY statement T_CATCH statement  */
#line 763 "engines/scumm/editor/compiler/parser.y"
        {
		Statement *catchStatement = statementCollector.back();
		statementCollector.pop_back();
		Statement *tryStatement = statementCollector.back();
		statementCollector.pop_back();
		TryCatchFinallyStatement *tryCatchStatement = new TryCatchFinallyStatement(tryStatement, catchStatement, NULL);
		statementCollector.push_back(tryCatchStatement);
	}
#line 2250 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 87: /* statement: T_TRY statement T_FINALLY statement  */
#line 772 "engines/scumm/editor/compiler/parser.y"
        {
		Statement *finallyStatement = statementCollector.back();
		statementCollector.pop_back();
		Statement *tryStatement = statementCollector.back();
		statementCollector.pop_back();
		TryCatchFinallyStatement *tryFinallyStatement = new TryCatchFinallyStatement(tryStatement, NULL, finallyStatement);
		statementCollector.push_back(tryFinallyStatement);
	}
#line 2263 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 88: /* statement: T_TRY statement T_CATCH statement T_FINALLY statement  */
#line 781 "engines/scumm/editor/compiler/parser.y"
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
#line 2278 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 89: /* statement: T_ASSEMBLY '{' assemblyTokens '}'  */
#line 792 "engines/scumm/editor/compiler/parser.y"
        {
		AssemblyStatement *assemblyStatement = new AssemblyStatement();
		for (int i = 0; i < (int)assemblyTokens.size(); i++)
			assemblyStatement->addToken(assemblyTokens[i]);
		assemblyTokens.clear();
		statementCollector.push_back(assemblyStatement);
	}
#line 2290 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 90: /* statements: statements statement  */
#line 803 "engines/scumm/editor/compiler/parser.y"
        {
		Statement *statement = statementCollector.back();
		statementListCollector.back().push_back(statement);
		statementCollector.pop_back();
	}
#line 2300 "engines/scumm/editor/compiler/grammar.cpp"
    break;

  case 91: /* statements: %empty  */
#line 809 "engines/scumm/editor/compiler/parser.y"
        {
		Common::Array<Statement *> v;
		statementListCollector.push_back(v);
	}
#line 2309 "engines/scumm/editor/compiler/grammar.cpp"
    break;


#line 2313 "engines/scumm/editor/compiler/grammar.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == SCUMM_YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (declarations, functions, YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= SCUMM_YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == SCUMM_YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, declarations, functions);
          yychar = SCUMM_YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, declarations, functions);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (declarations, functions, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != SCUMM_YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, declarations, functions);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, declarations, functions);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 815 "engines/scumm/editor/compiler/parser.y"


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
