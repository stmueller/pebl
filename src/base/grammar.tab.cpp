/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Skeleton implementation for Bison GLR parsers in C

   Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

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

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 0






/* First part of user prologue.  */
#line 1 "src/base/grammar.y"

  
#include "PNode.h"
#include <stdio.h>
#include <iostream>
#include <cctype>
#include <cstring>
#include <vector>
#include <stack>

//This is taken directly from Defs.h  
#ifdef PEBL_EMSCRIPTEN
#define pDouble double
#define pInt int
#else
#define pDouble long double
#define pInt long int
#endif

  // Prototypes to keep the compiler happy
  void yyerror (const char *error);
  int  yylex ();
  extern FILE * yyin;
  PNode * gParseTreeHead;
  PNode * parse(const char* filename);
  extern int yylineno;
  char* sourcefilename;



#line 89 "src/base/grammar.tab.cpp"

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

#include "grammar.tab.hpp"

/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_PEBL_AND = 3,                   /* PEBL_AND  */
  YYSYMBOL_PEBL_ADD = 4,                   /* PEBL_ADD  */
  YYSYMBOL_PEBL_ARGLIST = 5,               /* PEBL_ARGLIST  */
  YYSYMBOL_PEBL_ASSIGN = 6,                /* PEBL_ASSIGN  */
  YYSYMBOL_PEBL_BREAK = 7,                 /* PEBL_BREAK  */
  YYSYMBOL_PEBL_COMMA = 8,                 /* PEBL_COMMA  */
  YYSYMBOL_PEBL_DEFINE = 9,                /* PEBL_DEFINE  */
  YYSYMBOL_PEBL_DIVIDE = 10,               /* PEBL_DIVIDE  */
  YYSYMBOL_PEBL_DOT = 11,                  /* PEBL_DOT  */
  YYSYMBOL_PEBL_ELSE = 12,                 /* PEBL_ELSE  */
  YYSYMBOL_PEBL_ELSEIF = 13,               /* PEBL_ELSEIF  */
  YYSYMBOL_PEBL_END = 14,                  /* PEBL_END  */
  YYSYMBOL_PEBL_EOF = 15,                  /* PEBL_EOF  */
  YYSYMBOL_PEBL_EQ = 16,                   /* PEBL_EQ  */
  YYSYMBOL_PEBL_FUNCTION = 17,             /* PEBL_FUNCTION  */
  YYSYMBOL_PEBL_FUNCTIONS = 18,            /* PEBL_FUNCTIONS  */
  YYSYMBOL_PEBL_GE = 19,                   /* PEBL_GE  */
  YYSYMBOL_PEBL_GT = 20,                   /* PEBL_GT  */
  YYSYMBOL_PEBL_IF = 21,                   /* PEBL_IF  */
  YYSYMBOL_PEBL_IFELSE = 22,               /* PEBL_IFELSE  */
  YYSYMBOL_PEBL_LAMBDAFUNCTION = 23,       /* PEBL_LAMBDAFUNCTION  */
  YYSYMBOL_PEBL_LBRACE = 24,               /* PEBL_LBRACE  */
  YYSYMBOL_PEBL_LBRACKET = 25,             /* PEBL_LBRACKET  */
  YYSYMBOL_PEBL_LE = 26,                   /* PEBL_LE  */
  YYSYMBOL_PEBL_LIBRARYFUNCTION = 27,      /* PEBL_LIBRARYFUNCTION  */
  YYSYMBOL_PEBL_LISTHEAD = 28,             /* PEBL_LISTHEAD  */
  YYSYMBOL_PEBL_LISTITEM = 29,             /* PEBL_LISTITEM  */
  YYSYMBOL_PEBL_LOOP = 30,                 /* PEBL_LOOP  */
  YYSYMBOL_PEBL_LPAREN = 31,               /* PEBL_LPAREN  */
  YYSYMBOL_PEBL_LT = 32,                   /* PEBL_LT  */
  YYSYMBOL_PEBL_MULTIPLY = 33,             /* PEBL_MULTIPLY  */
  YYSYMBOL_PEBL_NE = 34,                   /* PEBL_NE  */
  YYSYMBOL_PEBL_NEWLINE = 35,              /* PEBL_NEWLINE  */
  YYSYMBOL_PEBL_NOT = 36,                  /* PEBL_NOT  */
  YYSYMBOL_PEBL_OR = 37,                   /* PEBL_OR  */
  YYSYMBOL_PEBL_POWER = 38,                /* PEBL_POWER  */
  YYSYMBOL_PEBL_RETURN = 39,               /* PEBL_RETURN  */
  YYSYMBOL_PEBL_RBRACE = 40,               /* PEBL_RBRACE  */
  YYSYMBOL_PEBL_RBRACKET = 41,             /* PEBL_RBRACKET  */
  YYSYMBOL_PEBL_RPAREN = 42,               /* PEBL_RPAREN  */
  YYSYMBOL_PEBL_SUBTRACT = 43,             /* PEBL_SUBTRACT  */
  YYSYMBOL_PEBL_SEMI = 44,                 /* PEBL_SEMI  */
  YYSYMBOL_PEBL_STATEMENTS = 45,           /* PEBL_STATEMENTS  */
  YYSYMBOL_PEBL_START = 46,                /* PEBL_START  */
  YYSYMBOL_PEBL_VARPAIR = 47,              /* PEBL_VARPAIR  */
  YYSYMBOL_PEBL_VARIABLEDATUM = 48,        /* PEBL_VARIABLEDATUM  */
  YYSYMBOL_PEBL_VARLIST = 49,              /* PEBL_VARLIST  */
  YYSYMBOL_PEBL_WHILE = 50,                /* PEBL_WHILE  */
  YYSYMBOL_PEBL_AND_TAIL = 51,             /* PEBL_AND_TAIL  */
  YYSYMBOL_PEBL_ADD_TAIL = 52,             /* PEBL_ADD_TAIL  */
  YYSYMBOL_PEBL_ASSIGN_TAIL = 53,          /* PEBL_ASSIGN_TAIL  */
  YYSYMBOL_PEBL_BREAK_TAIL = 54,           /* PEBL_BREAK_TAIL  */
  YYSYMBOL_PEBL_COLON = 55,                /* PEBL_COLON  */
  YYSYMBOL_PEBL_DIVIDE_TAIL = 56,          /* PEBL_DIVIDE_TAIL  */
  YYSYMBOL_PEBL_EQ_TAIL = 57,              /* PEBL_EQ_TAIL  */
  YYSYMBOL_PEBL_GE_TAIL = 58,              /* PEBL_GE_TAIL  */
  YYSYMBOL_PEBL_GT_TAIL = 59,              /* PEBL_GT_TAIL  */
  YYSYMBOL_PEBL_IF_TAIL = 60,              /* PEBL_IF_TAIL  */
  YYSYMBOL_PEBL_IF_TAIL2 = 61,             /* PEBL_IF_TAIL2  */
  YYSYMBOL_PEBL_ELSE_TAIL = 62,            /* PEBL_ELSE_TAIL  */
  YYSYMBOL_PEBL_LE_TAIL = 63,              /* PEBL_LE_TAIL  */
  YYSYMBOL_PEBL_LISTITEM_TAIL = 64,        /* PEBL_LISTITEM_TAIL  */
  YYSYMBOL_PEBL_LOOP_TAIL1 = 65,           /* PEBL_LOOP_TAIL1  */
  YYSYMBOL_PEBL_LOOP_TAIL2 = 66,           /* PEBL_LOOP_TAIL2  */
  YYSYMBOL_PEBL_LT_TAIL = 67,              /* PEBL_LT_TAIL  */
  YYSYMBOL_PEBL_MULTIPLY_TAIL = 68,        /* PEBL_MULTIPLY_TAIL  */
  YYSYMBOL_PEBL_NE_TAIL = 69,              /* PEBL_NE_TAIL  */
  YYSYMBOL_PEBL_NOT_TAIL = 70,             /* PEBL_NOT_TAIL  */
  YYSYMBOL_PEBL_OR_TAIL = 71,              /* PEBL_OR_TAIL  */
  YYSYMBOL_PEBL_POWER_TAIL = 72,           /* PEBL_POWER_TAIL  */
  YYSYMBOL_PEBL_RETURN_TAIL = 73,          /* PEBL_RETURN_TAIL  */
  YYSYMBOL_PEBL_SUBTRACT_TAIL = 74,        /* PEBL_SUBTRACT_TAIL  */
  YYSYMBOL_PEBL_STATEMENTS_TAIL1 = 75,     /* PEBL_STATEMENTS_TAIL1  */
  YYSYMBOL_PEBL_STATEMENTS_TAIL2 = 76,     /* PEBL_STATEMENTS_TAIL2  */
  YYSYMBOL_PEBL_WHILE_TAIL = 77,           /* PEBL_WHILE_TAIL  */
  YYSYMBOL_PEBL_WHILE_TAIL2 = 78,          /* PEBL_WHILE_TAIL2  */
  YYSYMBOL_PEBL_FUNCTION_TAIL1 = 79,       /* PEBL_FUNCTION_TAIL1  */
  YYSYMBOL_PEBL_FUNCTION_TAIL2 = 80,       /* PEBL_FUNCTION_TAIL2  */
  YYSYMBOL_PEBL_FUNCTION_TAIL_LIBFUNCTION = 81, /* PEBL_FUNCTION_TAIL_LIBFUNCTION  */
  YYSYMBOL_PEBL_FLOAT = 82,                /* PEBL_FLOAT  */
  YYSYMBOL_PEBL_INTEGER = 83,              /* PEBL_INTEGER  */
  YYSYMBOL_PEBL_STRING = 84,               /* PEBL_STRING  */
  YYSYMBOL_PEBL_SYMBOL = 85,               /* PEBL_SYMBOL  */
  YYSYMBOL_PEBL_LOCALVAR = 86,             /* PEBL_LOCALVAR  */
  YYSYMBOL_PEBL_GLOBALVAR = 87,            /* PEBL_GLOBALVAR  */
  YYSYMBOL_PEBL_FUNCTIONNAME = 88,         /* PEBL_FUNCTIONNAME  */
  YYSYMBOL_PEBL_UMINUS = 89,               /* PEBL_UMINUS  */
  YYSYMBOL_YYACCEPT = 90,                  /* $accept  */
  YYSYMBOL_functions = 91,                 /* functions  */
  YYSYMBOL_function = 92,                  /* function  */
  YYSYMBOL_functionblock = 93,             /* functionblock  */
  YYSYMBOL_functionsequence = 94,          /* functionsequence  */
  YYSYMBOL_block = 95,                     /* block  */
  YYSYMBOL_sequence = 96,                  /* sequence  */
  YYSYMBOL_statement = 97,                 /* statement  */
  YYSYMBOL_endstatement = 98,              /* endstatement  */
  YYSYMBOL_ustatement = 99,                /* ustatement  */
  YYSYMBOL_returnstatement = 100,          /* returnstatement  */
  YYSYMBOL_endreturnstatement = 101,       /* endreturnstatement  */
  YYSYMBOL_elseifseq_or_nothing = 102,     /* elseifseq_or_nothing  */
  YYSYMBOL_elseifseq = 103,                /* elseifseq  */
  YYSYMBOL_arglist = 104,                  /* arglist  */
  YYSYMBOL_list = 105,                     /* list  */
  YYSYMBOL_explist = 106,                  /* explist  */
  YYSYMBOL_varlist = 107,                  /* varlist  */
  YYSYMBOL_variablepair = 108,             /* variablepair  */
  YYSYMBOL_exp = 109,                      /* exp  */
  YYSYMBOL_datum = 110,                    /* datum  */
  YYSYMBOL_variable = 111,                 /* variable  */
  YYSYMBOL_nlornone = 112,                 /* nlornone  */
  YYSYMBOL_newlines = 113                  /* newlines  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template, here we set
   the default value of $$ to a zeroed-out value.  Since the default
   value is undefined, this behavior is technically correct.  */
static YYSTYPE yyval_default;



#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#ifdef __cplusplus
  typedef bool yybool;
# define yytrue true
# define yyfalse false
#else
  /* When we move to stdbool, get rid of the various casts to yybool.  */
  typedef signed char yybool;
# define yytrue 1
# define yyfalse 0
#endif

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(Env) setjmp (Env)
/* Pacify Clang and ICC.  */
# define YYLONGJMP(Env, Val)                    \
 do {                                           \
   longjmp (Env, Val);                          \
   YY_ASSERT (0);                               \
 } while (yyfalse)
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

/* The _Noreturn keyword of C11.  */
#ifndef _Noreturn
# if (defined __cplusplus \
      && ((201103 <= __cplusplus && !(__GNUC__ == 4 && __GNUC_MINOR__ == 7)) \
          || (defined _MSC_VER && 1900 <= _MSC_VER)))
#  define _Noreturn [[noreturn]]
# elif ((!defined __cplusplus || defined __clang__) \
        && (201112 <= (defined __STDC_VERSION__ ? __STDC_VERSION__ : 0) \
            || (!defined __STRICT_ANSI__ \
                && (4 < __GNUC__ + (7 <= __GNUC_MINOR__) \
                    || (defined __apple_build_version__ \
                        ? 6000000 <= __apple_build_version__ \
                        : 3 < __clang_major__ + (5 <= __clang_minor__))))))
   /* _Noreturn works as-is.  */
# elif (2 < __GNUC__ + (8 <= __GNUC_MINOR__) || defined __clang__ \
        || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif 1200 <= (defined _MSC_VER ? _MSC_VER : 0)
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
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

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   549

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  90
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  73
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  168
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 8
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   344

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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89
};

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   166,   166,   174,   182,   192,   207,   216,   220,   221,
     222,   223,   227,   228,   229,   231,   237,   240,   244,   248,
     257,   259,   261,   270,   278,   291,   302,   316,   324,   328,
     332,   333,   337,   344,   350,   367,   371,   381,   384,   388,
     392,   400,   404,   411,   413,   421,   424,   427,   433,   436,
     440,   443,   446,   449,   452,   455,   459,   462,   465,   468,
     471,   474,   483,   497,   500,   503,   510,   513,   518,   523,
     532,   533,   537,   538
};
#endif

#define YYPACT_NINF (-102)
#define YYTABLE_NINF (-34)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -27,  -102,     9,     3,   -11,  -102,   -62,   -27,   -27,  -102,
      -3,  -102,  -102,   -26,   -27,  -102,  -102,    -6,    23,   -21,
      16,   -27,   -27,   245,   -27,  -102,  -102,    16,   -76,   -27,
     -27,   245,   245,  -102,  -102,  -102,    19,  -102,   486,  -102,
    -102,   143,  -102,  -102,    -4,   245,   506,    18,   -22,  -102,
     -27,   -27,   -27,   245,   245,   245,   245,   245,   -27,   245,
     -27,   -27,   -27,    27,    35,    39,   264,  -102,    40,    61,
      66,    34,   -27,  -102,  -102,    -2,   -27,  -102,   486,  -102,
     -27,   350,   370,  -102,   245,   245,   245,   245,    25,    25,
      25,    25,    25,   245,    25,   245,   245,   245,  -102,   245,
     -76,  -102,    41,   245,   245,   245,  -102,   169,  -102,  -102,
    -102,    44,   -27,    45,   -27,   506,    -8,    18,    18,   506,
    -102,    -8,   396,    69,   416,   486,   486,  -102,  -102,  -102,
     -27,  -102,  -102,   245,  -102,    46,   -27,   245,   -27,  -102,
    -102,  -102,    62,   441,    62,   -27,     7,   -27,  -102,   229,
    -102,    36,    62,   -27,   -27,    63,  -102,  -102,   237,    62,
     245,  -102,   461,   -27,    62,    24,    36,  -102
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      70,    72,     0,     0,    71,     1,     0,    70,    70,    73,
       0,     3,     2,     0,    70,    68,    69,     0,    41,    43,
       0,    70,    70,     0,    70,     5,     6,     0,     0,    70,
      70,     0,     0,    64,    63,    65,     0,    66,    44,    45,
      67,     0,     4,    42,     0,     0,    48,    47,    70,    62,
      70,    70,    70,     0,     0,     0,     0,     0,    70,     0,
      70,    70,    70,     0,     0,     0,     0,    15,     0,    68,
      69,     0,    70,    16,    13,     0,    70,    10,    20,    38,
      70,    39,    70,    35,     0,     0,     0,     0,    60,    58,
      57,    59,    56,     0,    61,     0,     0,     0,    21,     0,
       0,    28,    29,     0,     0,     0,     7,     0,    18,    19,
       8,     0,    70,     0,    70,    55,    49,    50,    51,    54,
      52,    53,     0,     0,     0,    22,    23,    12,    17,    14,
      70,    11,    37,     0,    46,     0,    70,     0,    70,     9,
      40,    36,     0,     0,     0,    70,    70,    70,    24,     0,
      25,     0,     0,    70,    70,     0,    31,    27,     0,     0,
       0,    32,     0,    70,     0,    70,     0,    34
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -102,  -102,    89,    68,  -102,   -43,   -56,   -61,  -101,    30,
      -1,     1,  -102,   -68,  -102,  -102,   -81,    72,  -102,    58,
    -102,    -9,    -7,  -102
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     7,    25,    71,    26,    72,    73,    74,    75,
      76,    77,   150,   156,    49,    37,    80,    17,    18,    78,
      39,    40,     3,     4
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      11,    12,    52,   114,    19,   101,   129,    20,     1,     5,
      15,    16,     6,     1,    27,    28,    14,    41,     6,    19,
      83,    29,    44,    45,     9,    58,    10,    30,    13,    51,
      61,    22,    31,   108,    23,    52,    21,    79,   109,    32,
      24,    84,     1,    85,    86,    87,   128,   -26,   154,   155,
      48,    93,   140,    95,    96,    97,    61,   129,    58,     1,
      15,    16,    98,    61,   -33,   107,    99,   104,    62,   110,
     100,   103,   105,   111,   106,   113,   108,   137,    33,    34,
      35,    38,    15,    16,    36,   132,   145,   134,   141,    46,
      47,   123,     8,   153,   160,    42,   102,   128,   167,   146,
      43,   148,    81,    82,     0,   133,   130,   135,   131,   157,
       0,    88,    89,    90,    91,    92,   161,    94,     0,     0,
       0,   165,     0,   139,     0,     0,     0,     0,     0,   142,
       0,   144,     0,     0,     0,     0,     0,     0,   149,   151,
     152,     0,    81,   115,   116,   117,   158,   159,     0,     0,
      63,   118,     0,   119,   120,   121,   164,   122,   166,     0,
       0,   124,   125,   126,    64,     0,     0,     0,    29,     0,
       0,     0,     0,    65,    30,     0,    63,     0,     0,    31,
       0,     0,    66,    67,     0,     0,    32,     0,     0,     0,
      64,    81,     0,    68,    29,   143,     0,     0,     0,    65,
      30,     0,     0,     0,     0,    31,     0,     0,    66,   127,
       0,     0,    32,     0,     0,     0,     0,     0,   162,    68,
       0,     0,     0,     0,     0,    33,    34,    35,     0,    69,
      70,    36,     0,     0,     0,     0,    63,     0,     0,     0,
       0,     0,     0,     0,    63,     0,     0,     0,     0,     0,
      64,    33,    34,    35,    29,    69,    70,    36,    64,    65,
      30,     0,    29,     0,     0,    31,     0,    65,    30,    67,
      29,    63,    32,    31,     0,     0,    30,   127,     0,    68,
      32,    31,     0,     0,     0,    64,     0,    68,    32,    29,
       0,     0,     0,     0,    65,    30,     0,     0,     0,     0,
      31,     0,     0,     0,     0,     0,     0,    32,     0,     0,
       0,    33,    34,    35,    68,    69,    70,    36,     0,    33,
      34,    35,     0,    69,    70,    36,     0,    33,    34,    35,
       0,    15,    16,    36,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,     0,
      69,    70,    36,    50,    51,     0,     0,     0,   112,     0,
      52,     0,     0,     0,     0,     0,    53,     0,     0,    54,
      55,     0,     0,    50,    51,     0,    56,     0,     0,     0,
      52,     0,    57,    58,    59,     0,    53,    60,    61,    54,
      55,     0,     0,    62,     0,     0,    56,     0,     0,    50,
      51,     0,    57,    58,    59,     1,    52,    60,    61,     0,
       0,     0,    53,    62,     0,    54,    55,     0,     0,    50,
      51,     0,    56,     0,     0,     0,    52,     0,    57,    58,
      59,     0,    53,    60,    61,    54,    55,     0,   136,    62,
       0,     0,    56,     0,    50,    51,     0,     0,    57,    58,
      59,    52,     0,    60,    61,     0,     0,    53,   138,    62,
      54,    55,     0,     0,    50,    51,     0,    56,     0,     0,
       0,    52,     0,    57,    58,    59,     0,    53,    60,    61,
      54,    55,     0,   147,    62,     0,     0,    56,     0,    50,
      51,     0,     0,    57,    58,    59,    52,     0,    60,    61,
       0,     0,    53,   163,    62,    54,    55,     0,     0,     0,
      51,     0,    56,     0,     0,     0,    52,     0,    57,    58,
      59,     0,    53,    60,    61,    54,    55,     0,     0,    62,
       0,     0,    56,     0,     0,     0,     0,     0,    57,    58,
      59,     0,     0,     0,    61,     0,     0,     0,     0,    62
};

static const yytype_int16 yycheck[] =
{
       7,     8,    10,    84,    13,    66,   107,    14,    35,     0,
      86,    87,     9,    35,    21,    22,    42,    24,     9,    28,
      42,    25,    29,    30,    35,    33,    88,    31,    31,     4,
      38,     8,    36,    35,    55,    10,    42,    41,    40,    43,
      24,    48,    35,    50,    51,    52,   107,    40,    12,    13,
      31,    58,   133,    60,    61,    62,    38,   158,    33,    35,
      86,    87,    35,    38,    40,    72,    31,     6,    43,    76,
      31,    31,     6,    80,    40,    82,    35,     8,    82,    83,
      84,    23,    86,    87,    88,    41,    24,    42,    42,    31,
      32,   100,     3,   149,    31,    27,    66,   158,   166,   142,
      28,   144,    44,    45,    -1,   112,   107,   114,   107,   152,
      -1,    53,    54,    55,    56,    57,   159,    59,    -1,    -1,
      -1,   164,    -1,   130,    -1,    -1,    -1,    -1,    -1,   136,
      -1,   138,    -1,    -1,    -1,    -1,    -1,    -1,   145,   146,
     147,    -1,    84,    85,    86,    87,   153,   154,    -1,    -1,
       7,    93,    -1,    95,    96,    97,   163,    99,   165,    -1,
      -1,   103,   104,   105,    21,    -1,    -1,    -1,    25,    -1,
      -1,    -1,    -1,    30,    31,    -1,     7,    -1,    -1,    36,
      -1,    -1,    39,    40,    -1,    -1,    43,    -1,    -1,    -1,
      21,   133,    -1,    50,    25,   137,    -1,    -1,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    39,    40,
      -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,   160,    50,
      -1,    -1,    -1,    -1,    -1,    82,    83,    84,    -1,    86,
      87,    88,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      21,    82,    83,    84,    25,    86,    87,    88,    21,    30,
      31,    -1,    25,    -1,    -1,    36,    -1,    30,    31,    40,
      25,     7,    43,    36,    -1,    -1,    31,    40,    -1,    50,
      43,    36,    -1,    -1,    -1,    21,    -1,    50,    43,    25,
      -1,    -1,    -1,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    -1,
      -1,    82,    83,    84,    50,    86,    87,    88,    -1,    82,
      83,    84,    -1,    86,    87,    88,    -1,    82,    83,    84,
      -1,    86,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    83,    84,    -1,
      86,    87,    88,     3,     4,    -1,    -1,    -1,     8,    -1,
      10,    -1,    -1,    -1,    -1,    -1,    16,    -1,    -1,    19,
      20,    -1,    -1,     3,     4,    -1,    26,    -1,    -1,    -1,
      10,    -1,    32,    33,    34,    -1,    16,    37,    38,    19,
      20,    -1,    -1,    43,    -1,    -1,    26,    -1,    -1,     3,
       4,    -1,    32,    33,    34,    35,    10,    37,    38,    -1,
      -1,    -1,    16,    43,    -1,    19,    20,    -1,    -1,     3,
       4,    -1,    26,    -1,    -1,    -1,    10,    -1,    32,    33,
      34,    -1,    16,    37,    38,    19,    20,    -1,    42,    43,
      -1,    -1,    26,    -1,     3,     4,    -1,    -1,    32,    33,
      34,    10,    -1,    37,    38,    -1,    -1,    16,    42,    43,
      19,    20,    -1,    -1,     3,     4,    -1,    26,    -1,    -1,
      -1,    10,    -1,    32,    33,    34,    -1,    16,    37,    38,
      19,    20,    -1,    42,    43,    -1,    -1,    26,    -1,     3,
       4,    -1,    -1,    32,    33,    34,    10,    -1,    37,    38,
      -1,    -1,    16,    42,    43,    19,    20,    -1,    -1,    -1,
       4,    -1,    26,    -1,    -1,    -1,    10,    -1,    32,    33,
      34,    -1,    16,    37,    38,    19,    20,    -1,    -1,    43,
      -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    32,    33,
      34,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    35,    91,   112,   113,     0,     9,    92,    92,    35,
      88,   112,   112,    31,    42,    86,    87,   107,   108,   111,
     112,    42,     8,    55,    24,    93,    95,   112,   112,    25,
      31,    36,    43,    82,    83,    84,    88,   105,   109,   110,
     111,   112,    93,   107,   112,   112,   109,   109,    31,   104,
       3,     4,    10,    16,    19,    20,    26,    32,    33,    34,
      37,    38,    43,     7,    21,    30,    39,    40,    50,    86,
      87,    94,    96,    97,    98,    99,   100,   101,   109,    41,
     106,   109,   109,    42,   112,   112,   112,   112,   109,   109,
     109,   109,   109,   112,   109,   112,   112,   112,    35,    31,
      31,    97,    99,    31,     6,     6,    40,   112,    35,    40,
     112,   112,     8,   112,   106,   109,   109,   109,   109,   109,
     109,   109,   109,   111,   109,   109,   109,    40,    97,    98,
     100,   101,    41,   112,    42,   112,    42,     8,    42,   112,
     106,    42,   112,   109,   112,    24,    95,    42,    95,   112,
     102,   112,   112,    96,    12,    13,   103,    95,   112,   112,
      31,    95,   109,    42,   112,    95,   112,   103
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    90,    91,    91,    92,    92,    93,    93,    94,    94,
      94,    94,    95,    95,    95,    95,    96,    96,    97,    98,
      99,    99,    99,    99,    99,    99,    99,    99,   100,   101,
     102,   102,   103,   103,   103,   104,   104,   105,   105,   106,
     106,   107,   107,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   110,   110,   110,   110,   110,   111,   111,
     112,   112,   113,   113
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     3,     7,     6,     1,     4,     2,     4,
       1,     3,     5,     3,     5,     3,     1,     3,     2,     2,
       1,     2,     3,     3,     6,     7,     6,     8,     2,     2,
       0,     2,     3,     6,     8,     2,     5,     5,     3,     1,
       4,     1,     4,     1,     3,     1,     5,     2,     2,     4,
       4,     4,     4,     4,     4,     4,     3,     3,     3,     3,
       3,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     1,     2
};


/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
static const yytype_int8 yydprec[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     2,     1,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const yytype_int8 yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0
};

/* YYIMMEDIATE[RULE-NUM] -- True iff rule #RULE-NUM is not to be deferred, as
   in the case of predicates.  */
static const yybool yyimmediate[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const yytype_int8 yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     0,     0,     0,     0,     4,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     6,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short yyconfl[] =
{
       0,    26,    30,     0,    30,     0,    33,     0
};



YYSTYPE yylval;

int yynerrs;
int yychar;

enum { YYENOMEM = -2 };

typedef enum { yyok, yyaccept, yyabort, yyerr, yynomem } YYRESULTTAG;

#define YYCHK(YYE)                              \
  do {                                          \
    YYRESULTTAG yychk_flag = YYE;               \
    if (yychk_flag != yyok)                     \
      return yychk_flag;                        \
  } while (0)

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
#  define YYSTACKEXPANDABLE 1
#endif

#if YYSTACKEXPANDABLE
# define YY_RESERVE_GLRSTACK(Yystack)                   \
  do {                                                  \
    if (Yystack->yyspaceLeft < YYHEADROOM)              \
      yyexpandGLRStack (Yystack);                       \
  } while (0)
#else
# define YY_RESERVE_GLRSTACK(Yystack)                   \
  do {                                                  \
    if (Yystack->yyspaceLeft < YYHEADROOM)              \
      yyMemoryExhausted (Yystack);                      \
  } while (0)
#endif

/** State numbers. */
typedef int yy_state_t;

/** Rule numbers. */
typedef int yyRuleNum;

/** Item references. */
typedef short yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState
{
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yyval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yy_state_t yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the last token produced by my symbol */
  YYPTRDIFF_T yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  nonterminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yyval;
  } yysemantics;
};

struct yyGLRStateSet
{
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  YYPTRDIFF_T yysize;
  YYPTRDIFF_T yycapacity;
};

struct yySemanticOption
{
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrState;


  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  YYPTRDIFF_T yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack (yyGLRStack* yystackp);
#endif

_Noreturn static void
yyFail (yyGLRStack* yystackp, const char* yymsg)
{
  if (yymsg != YY_NULLPTR)
    yyerror (yymsg);
  YYLONGJMP (yystackp->yyexception_buffer, 1);
}

_Noreturn static void
yyMemoryExhausted (yyGLRStack* yystackp)
{
  YYLONGJMP (yystackp->yyexception_buffer, 2);
}

/** Accessing symbol of state YYSTATE.  */
static inline yysymbol_kind_t
yy_accessing_symbol (yy_state_t yystate)
{
  return YY_CAST (yysymbol_kind_t, yystos[yystate]);
}

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "PEBL_AND", "PEBL_ADD",
  "PEBL_ARGLIST", "PEBL_ASSIGN", "PEBL_BREAK", "PEBL_COMMA", "PEBL_DEFINE",
  "PEBL_DIVIDE", "PEBL_DOT", "PEBL_ELSE", "PEBL_ELSEIF", "PEBL_END",
  "PEBL_EOF", "PEBL_EQ", "PEBL_FUNCTION", "PEBL_FUNCTIONS", "PEBL_GE",
  "PEBL_GT", "PEBL_IF", "PEBL_IFELSE", "PEBL_LAMBDAFUNCTION",
  "PEBL_LBRACE", "PEBL_LBRACKET", "PEBL_LE", "PEBL_LIBRARYFUNCTION",
  "PEBL_LISTHEAD", "PEBL_LISTITEM", "PEBL_LOOP", "PEBL_LPAREN", "PEBL_LT",
  "PEBL_MULTIPLY", "PEBL_NE", "PEBL_NEWLINE", "PEBL_NOT", "PEBL_OR",
  "PEBL_POWER", "PEBL_RETURN", "PEBL_RBRACE", "PEBL_RBRACKET",
  "PEBL_RPAREN", "PEBL_SUBTRACT", "PEBL_SEMI", "PEBL_STATEMENTS",
  "PEBL_START", "PEBL_VARPAIR", "PEBL_VARIABLEDATUM", "PEBL_VARLIST",
  "PEBL_WHILE", "PEBL_AND_TAIL", "PEBL_ADD_TAIL", "PEBL_ASSIGN_TAIL",
  "PEBL_BREAK_TAIL", "PEBL_COLON", "PEBL_DIVIDE_TAIL", "PEBL_EQ_TAIL",
  "PEBL_GE_TAIL", "PEBL_GT_TAIL", "PEBL_IF_TAIL", "PEBL_IF_TAIL2",
  "PEBL_ELSE_TAIL", "PEBL_LE_TAIL", "PEBL_LISTITEM_TAIL",
  "PEBL_LOOP_TAIL1", "PEBL_LOOP_TAIL2", "PEBL_LT_TAIL",
  "PEBL_MULTIPLY_TAIL", "PEBL_NE_TAIL", "PEBL_NOT_TAIL", "PEBL_OR_TAIL",
  "PEBL_POWER_TAIL", "PEBL_RETURN_TAIL", "PEBL_SUBTRACT_TAIL",
  "PEBL_STATEMENTS_TAIL1", "PEBL_STATEMENTS_TAIL2", "PEBL_WHILE_TAIL",
  "PEBL_WHILE_TAIL2", "PEBL_FUNCTION_TAIL1", "PEBL_FUNCTION_TAIL2",
  "PEBL_FUNCTION_TAIL_LIBFUNCTION", "PEBL_FLOAT", "PEBL_INTEGER",
  "PEBL_STRING", "PEBL_SYMBOL", "PEBL_LOCALVAR", "PEBL_GLOBALVAR",
  "PEBL_FUNCTIONNAME", "PEBL_UMINUS", "$accept", "functions", "function",
  "functionblock", "functionsequence", "block", "sequence", "statement",
  "endstatement", "ustatement", "returnstatement", "endreturnstatement",
  "elseifseq_or_nothing", "elseifseq", "arglist", "list", "explist",
  "varlist", "variablepair", "exp", "datum", "variable", "nlornone",
  "newlines", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

/** Left-hand-side symbol for rule #YYRULE.  */
static inline yysymbol_kind_t
yylhsNonterm (yyRuleNum yyrule)
{
  return YY_CAST (yysymbol_kind_t, yyr1[yyrule]);
}

#if YYDEBUG

# ifndef YYFPRINTF
#  define YYFPRINTF fprintf
# endif

# define YY_FPRINTF                             \
  YY_IGNORE_USELESS_CAST_BEGIN YY_FPRINTF_

# define YY_FPRINTF_(Args)                      \
  do {                                          \
    YYFPRINTF Args;                             \
    YY_IGNORE_USELESS_CAST_END                  \
  } while (0)

# define YY_DPRINTF                             \
  YY_IGNORE_USELESS_CAST_BEGIN YY_DPRINTF_

# define YY_DPRINTF_(Args)                      \
  do {                                          \
    if (yydebug)                                \
      YYFPRINTF Args;                           \
    YY_IGNORE_USELESS_CAST_END                  \
  } while (0)





/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
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
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                  \
  do {                                                                  \
    if (yydebug)                                                        \
      {                                                                 \
        YY_FPRINTF ((stderr, "%s ", Title));                            \
        yy_symbol_print (stderr, Kind, Value);        \
        YY_FPRINTF ((stderr, "\n"));                                    \
      }                                                                 \
  } while (0)

static inline void
yy_reduce_print (yybool yynormal, yyGLRStackItem* yyvsp, YYPTRDIFF_T yyk,
                 yyRuleNum yyrule);

# define YY_REDUCE_PRINT(Args)          \
  do {                                  \
    if (yydebug)                        \
      yy_reduce_print Args;             \
  } while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

static void yypstack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
  YY_ATTRIBUTE_UNUSED;
static void yypdumpstack (yyGLRStack* yystackp)
  YY_ATTRIBUTE_UNUSED;

#else /* !YYDEBUG */

# define YY_DPRINTF(Args) do {} while (yyfalse)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_REDUCE_PRINT(Args)

#endif /* !YYDEBUG */



/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain.  */
static void yyfillin (yyGLRStackItem *, int, int) YY_ATTRIBUTE_UNUSED;
static void
yyfillin (yyGLRStackItem *yyvsp, int yylow0, int yylow1)
{
  int i;
  yyGLRState *s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0-1; i >= yylow1; i -= 1)
    {
#if YYDEBUG
      yyvsp[i].yystate.yylrState = s->yylrState;
#endif
      yyvsp[i].yystate.yyresolved = s->yyresolved;
      if (s->yyresolved)
        yyvsp[i].yystate.yysemantics.yyval = s->yysemantics.yyval;
      else
        /* The effect of using yyval or yyloc (in an immediate rule) is
         * undefined.  */
        yyvsp[i].yystate.yysemantics.yyfirstVal = YY_NULLPTR;
      s = yyvsp[i].yystate.yypred = s->yypred;
    }
}


/** If yychar is empty, fetch the next token.  */
static inline yysymbol_kind_t
yygetToken (int *yycharp)
{
  yysymbol_kind_t yytoken;
  if (*yycharp == YYEMPTY)
    {
      YY_DPRINTF ((stderr, "Reading a token\n"));
      *yycharp = yylex ();
    }
  if (*yycharp <= YYEOF)
    {
      *yycharp = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YY_DPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (*yycharp);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }
  return yytoken;
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     YY_ATTRIBUTE_UNUSED;
static inline int
yyfill (yyGLRStackItem *yyvsp, int *yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
    {
      yyfillin (yyvsp, *yylow, yylow1);
      *yylow = yylow1;
    }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT, yynomem for YYNOMEM.  */
static YYRESULTTAG
yyuserAction (yyRuleNum yyrule, int yyrhslen, yyGLRStackItem* yyvsp,
              yyGLRStack* yystackp, YYPTRDIFF_T yyk,
              YYSTYPE* yyvalp)
{
  const yybool yynormal YY_ATTRIBUTE_UNUSED = yystackp->yysplitPoint == YY_NULLPTR;
  int yylow = 1;
  YY_USE (yyvalp);
  YY_USE (yyk);
  YY_USE (yyrhslen);
# undef yyerrok
# define yyerrok (yystackp->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYNOMEM
# define YYNOMEM return yynomem
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING() (yystackp->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, (N), yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)                                              \
  return yyerror (YY_("syntax error: cannot back up")),     \
         yyerrok, yyerr

  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yyval;
  /* If yyk == -1, we are running a deferred action on a temporary
     stack.  In that case, YY_REDUCE_PRINT must not play with YYFILL,
     so pretend the stack is "normal". */
  YY_REDUCE_PRINT ((yynormal || yyk == -1, yyvsp, yyk, yyrule));
  switch (yyrule)
    {
  case 2: /* functions: nlornone function nlornone  */
#line 166 "src/base/grammar.y"
                                           {gParseTreeHead =  new OpNode(PEBL_FUNCTIONS,(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp),NULL, sourcefilename, yylineno);

		           ((*yyvalp).exp) = gParseTreeHead;
		}
#line 1365 "src/base/grammar.tab.cpp"
    break;

  case 3: /* functions: functions function nlornone  */
#line 174 "src/base/grammar.y"
                                            { gParseTreeHead = new OpNode(PEBL_FUNCTIONS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);
		           ((*yyvalp).exp) = gParseTreeHead;
		}
#line 1373 "src/base/grammar.tab.cpp"
    break;

  case 4: /* function: PEBL_DEFINE PEBL_FUNCTIONNAME PEBL_LPAREN varlist PEBL_RPAREN nlornone functionblock  */
#line 182 "src/base/grammar.y"
                                                                                                       { ;
		PNode * tmpFN = new OpNode(PEBL_LAMBDAFUNCTION, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);  
		tmpFN->SetFunctionName((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-5)].yystate.yysemantics.yyval.function));
		PNode * tmpNode = new DataNode(Variant((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-5)].yystate.yysemantics.yyval.function), P_DATA_FUNCTION), sourcefilename, yylineno);
		free((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-5)].yystate.yysemantics.yyval.function));
		((*yyvalp).exp) = new OpNode(PEBL_FUNCTION, tmpNode, tmpFN, sourcefilename, yylineno);

        }
#line 1386 "src/base/grammar.tab.cpp"
    break;

  case 5: /* function: PEBL_DEFINE PEBL_FUNCTIONNAME PEBL_LPAREN PEBL_RPAREN nlornone functionblock  */
#line 192 "src/base/grammar.y"
                                                                                                 { ;
		PNode * tmpFN = new OpNode(PEBL_LAMBDAFUNCTION, NULL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno); 
		tmpFN->SetFunctionName((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yyval.function));
		PNode * tmpNode = new DataNode(Variant((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yyval.function), P_DATA_FUNCTION), sourcefilename, yylineno);
		((*yyvalp).exp) = new OpNode(PEBL_FUNCTION, tmpNode, tmpFN, sourcefilename, yylineno);
                free((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yyval.function));
		  }
#line 1398 "src/base/grammar.tab.cpp"
    break;

  case 6: /* functionblock: block  */
#line 207 "src/base/grammar.y"
                      {
		   /*When no return value is provided, return 1 (true)*/
                  DataNode* retval  = new DataNode (Variant(1), sourcefilename, yylineno);
		  OpNode *tmpReturn = new OpNode(PEBL_RETURN, retval, NULL, sourcefilename, yylineno);
    	           ((*yyvalp).exp) = new OpNode(PEBL_STATEMENTS,(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp),tmpReturn,sourcefilename,yylineno);
                 }
#line 1409 "src/base/grammar.tab.cpp"
    break;

  case 7: /* functionblock: PEBL_LBRACE nlornone functionsequence PEBL_RBRACE  */
#line 216 "src/base/grammar.y"
                                                                    {((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp);}
#line 1415 "src/base/grammar.tab.cpp"
    break;

  case 8: /* functionsequence: returnstatement nlornone  */
#line 220 "src/base/grammar.y"
                                                       { ((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp);}
#line 1421 "src/base/grammar.tab.cpp"
    break;

  case 9: /* functionsequence: sequence nlornone returnstatement nlornone  */
#line 221 "src/base/grammar.y"
                                                                { ((*yyvalp).exp) = new OpNode(PEBL_STATEMENTS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1427 "src/base/grammar.tab.cpp"
    break;

  case 10: /* functionsequence: endreturnstatement  */
#line 222 "src/base/grammar.y"
                                                             { ((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp);}
#line 1433 "src/base/grammar.tab.cpp"
    break;

  case 11: /* functionsequence: sequence nlornone endreturnstatement  */
#line 223 "src/base/grammar.y"
                                                             { ((*yyvalp).exp) = new OpNode(PEBL_STATEMENTS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1439 "src/base/grammar.tab.cpp"
    break;

  case 12: /* block: PEBL_LBRACE nlornone sequence nlornone PEBL_RBRACE  */
#line 227 "src/base/grammar.y"
                                                                     { ((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp);}
#line 1445 "src/base/grammar.tab.cpp"
    break;

  case 13: /* block: PEBL_LBRACE nlornone endstatement  */
#line 228 "src/base/grammar.y"
                                              {((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp);}
#line 1451 "src/base/grammar.tab.cpp"
    break;

  case 14: /* block: PEBL_LBRACE nlornone sequence nlornone endstatement  */
#line 229 "src/base/grammar.y"
                                                                 {
  ((*yyvalp).exp)  = new OpNode(PEBL_STATEMENTS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1458 "src/base/grammar.tab.cpp"
    break;

  case 15: /* block: PEBL_LBRACE nlornone PEBL_RBRACE  */
#line 231 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new DataNode (Variant(0), sourcefilename, yylineno);}
#line 1464 "src/base/grammar.tab.cpp"
    break;

  case 16: /* sequence: statement  */
#line 237 "src/base/grammar.y"
                                       { ((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp); }
#line 1470 "src/base/grammar.tab.cpp"
    break;

  case 17: /* sequence: sequence nlornone statement  */
#line 240 "src/base/grammar.y"
                                                   { ((*yyvalp).exp) = new OpNode(PEBL_STATEMENTS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1476 "src/base/grammar.tab.cpp"
    break;

  case 18: /* statement: ustatement PEBL_NEWLINE  */
#line 244 "src/base/grammar.y"
                                   {((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp);}
#line 1482 "src/base/grammar.tab.cpp"
    break;

  case 19: /* endstatement: ustatement PEBL_RBRACE  */
#line 248 "src/base/grammar.y"
                                     {((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp);}
#line 1488 "src/base/grammar.tab.cpp"
    break;

  case 20: /* ustatement: exp  */
#line 257 "src/base/grammar.y"
                                      {((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp);}
#line 1494 "src/base/grammar.tab.cpp"
    break;

  case 21: /* ustatement: PEBL_BREAK PEBL_NEWLINE  */
#line 259 "src/base/grammar.y"
                                          {((*yyvalp).exp) = new OpNode(PEBL_BREAK, NULL, NULL, sourcefilename, yylineno);}
#line 1500 "src/base/grammar.tab.cpp"
    break;

  case 22: /* ustatement: PEBL_LOCALVAR PEBL_ASSIGN exp  */
#line 262 "src/base/grammar.y"
                { 
	         Variant tmpV((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.symbol),P_DATA_LOCALVARIABLE);       /*create a new temporary variant*/
		 /*free($1);*/
		 PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);        /*create basic pnode*/
		 ((*yyvalp).exp) = new OpNode(PEBL_ASSIGN, tmpNode, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);   /*Use symbol node in assignment node*/
		}
#line 1511 "src/base/grammar.tab.cpp"
    break;

  case 23: /* ustatement: PEBL_GLOBALVAR PEBL_ASSIGN exp  */
#line 271 "src/base/grammar.y"
                { 
	        Variant tmpV((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.symbol),P_DATA_GLOBALVARIABLE);      /*create a new temporary variant*/
		PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);        /*create basic pnode*/
		((*yyvalp).exp) = new OpNode(PEBL_ASSIGN, tmpNode, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);   /*Use symbol node in assignment node*/
		}
#line 1521 "src/base/grammar.tab.cpp"
    break;

  case 24: /* ustatement: PEBL_WHILE PEBL_LPAREN exp PEBL_RPAREN nlornone block  */
#line 278 "src/base/grammar.y"
                                                                       {;
		((*yyvalp).exp) = new OpNode(PEBL_WHILE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno); }
#line 1528 "src/base/grammar.tab.cpp"
    break;

  case 25: /* ustatement: PEBL_IF PEBL_LPAREN exp PEBL_RPAREN nlornone block elseifseq_or_nothing  */
#line 291 "src/base/grammar.y"
                                                                                        {
		if((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp) != NULL) {
		    /*Has else/elseif - create IFELSE node*/
		    PNode * tmpNode = new OpNode(PEBL_ELSE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);
		    ((*yyvalp).exp) = new OpNode(PEBL_IFELSE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yyval.exp), tmpNode, sourcefilename, yylineno);
		} else {
		    /*No else - create simple IF node*/
		    ((*yyvalp).exp) = new OpNode(PEBL_IF, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);
		}
	}
#line 1543 "src/base/grammar.tab.cpp"
    break;

  case 26: /* ustatement: PEBL_IF PEBL_LPAREN exp PEBL_RPAREN nlornone block  */
#line 302 "src/base/grammar.y"
                                                                   {
		((*yyvalp).exp) = new OpNode(PEBL_IF, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);
	}
#line 1551 "src/base/grammar.tab.cpp"
    break;

  case 27: /* ustatement: PEBL_LOOP PEBL_LPAREN variable PEBL_COMMA exp PEBL_RPAREN nlornone block  */
#line 316 "src/base/grammar.y"
                                                                                          {
		PNode * tmpNode = new OpNode(PEBL_VARIABLEDATUM, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-5)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);
		((*yyvalp).exp) = new OpNode(PEBL_LOOP, tmpNode, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno); }
#line 1559 "src/base/grammar.tab.cpp"
    break;

  case 28: /* returnstatement: PEBL_RETURN statement  */
#line 324 "src/base/grammar.y"
                                          {((*yyvalp).exp) = new OpNode(PEBL_RETURN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), NULL, sourcefilename, yylineno);}
#line 1565 "src/base/grammar.tab.cpp"
    break;

  case 29: /* endreturnstatement: PEBL_RETURN ustatement  */
#line 328 "src/base/grammar.y"
                                              {((*yyvalp).exp) = new OpNode(PEBL_RETURN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), NULL, sourcefilename, yylineno);}
#line 1571 "src/base/grammar.tab.cpp"
    break;

  case 30: /* elseifseq_or_nothing: %empty  */
#line 332 "src/base/grammar.y"
                    { ((*yyvalp).exp) = NULL; }
#line 1577 "src/base/grammar.tab.cpp"
    break;

  case 31: /* elseifseq_or_nothing: nlornone elseifseq  */
#line 333 "src/base/grammar.y"
                             { ((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp); }
#line 1583 "src/base/grammar.tab.cpp"
    break;

  case 32: /* elseifseq: PEBL_ELSE nlornone block  */
#line 337 "src/base/grammar.y"
                                      {

		((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp); }
#line 1591 "src/base/grammar.tab.cpp"
    break;

  case 33: /* elseifseq: PEBL_ELSEIF PEBL_LPAREN exp PEBL_RPAREN nlornone block  */
#line 344 "src/base/grammar.y"
                                                                {
		/*First make the else node*/

		  ((*yyvalp).exp) =  new OpNode(PEBL_IF, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);

   }
#line 1602 "src/base/grammar.tab.cpp"
    break;

  case 34: /* elseifseq: PEBL_ELSEIF PEBL_LPAREN exp PEBL_RPAREN nlornone block nlornone elseifseq  */
#line 350 "src/base/grammar.y"
                                                                                 {

		/*First make the else node*/
		PNode * tmpNode = new OpNode(PEBL_ELSE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);
		/*Put the else node in the IF node*/
		((*yyvalp).exp) = new OpNode(PEBL_IFELSE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-5)].yystate.yysemantics.yyval.exp), tmpNode, sourcefilename, yylineno); }
#line 1613 "src/base/grammar.tab.cpp"
    break;

  case 35: /* arglist: PEBL_LPAREN PEBL_RPAREN  */
#line 367 "src/base/grammar.y"
                                                            {((*yyvalp).exp) = new OpNode(PEBL_ARGLIST, NULL, NULL, sourcefilename, yylineno);}
#line 1619 "src/base/grammar.tab.cpp"
    break;

  case 36: /* arglist: PEBL_LPAREN nlornone explist nlornone PEBL_RPAREN  */
#line 371 "src/base/grammar.y"
                                                                     {((*yyvalp).exp) = new OpNode(PEBL_ARGLIST, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), NULL, sourcefilename, yylineno);}
#line 1625 "src/base/grammar.tab.cpp"
    break;

  case 37: /* list: PEBL_LBRACKET nlornone explist nlornone PEBL_RBRACKET  */
#line 381 "src/base/grammar.y"
                                                                      {((*yyvalp).exp) = new OpNode(PEBL_LISTHEAD,(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), NULL, sourcefilename, yylineno);}
#line 1631 "src/base/grammar.tab.cpp"
    break;

  case 38: /* list: PEBL_LBRACKET nlornone PEBL_RBRACKET  */
#line 384 "src/base/grammar.y"
                                                               {((*yyvalp).exp) = new OpNode(PEBL_LISTHEAD, NULL, NULL, sourcefilename, yylineno);}
#line 1637 "src/base/grammar.tab.cpp"
    break;

  case 39: /* explist: exp  */
#line 388 "src/base/grammar.y"
                                                      {((*yyvalp).exp) = new OpNode(PEBL_LISTITEM, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), NULL, sourcefilename, yylineno);}
#line 1643 "src/base/grammar.tab.cpp"
    break;

  case 40: /* explist: exp PEBL_COMMA nlornone explist  */
#line 392 "src/base/grammar.y"
                                                      {((*yyvalp).exp) = new OpNode(PEBL_LISTITEM, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1649 "src/base/grammar.tab.cpp"
    break;

  case 41: /* varlist: variablepair  */
#line 400 "src/base/grammar.y"
                                  {((*yyvalp).exp) = new OpNode(PEBL_VARLIST, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), NULL, sourcefilename, yylineno);}
#line 1655 "src/base/grammar.tab.cpp"
    break;

  case 42: /* varlist: variablepair PEBL_COMMA nlornone varlist  */
#line 404 "src/base/grammar.y"
                                                           {((*yyvalp).exp) = new OpNode(PEBL_VARLIST,(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp),(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1661 "src/base/grammar.tab.cpp"
    break;

  case 43: /* variablepair: variable  */
#line 411 "src/base/grammar.y"
                         {((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp);}
#line 1667 "src/base/grammar.tab.cpp"
    break;

  case 44: /* variablepair: variable PEBL_COLON exp  */
#line 413 "src/base/grammar.y"
                                       {
		       ((*yyvalp).exp)  = new OpNode(PEBL_VARPAIR,(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp),sourcefilename,yylineno);
	       }
#line 1675 "src/base/grammar.tab.cpp"
    break;

  case 45: /* exp: datum  */
#line 421 "src/base/grammar.y"
                                        { ((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp);}
#line 1681 "src/base/grammar.tab.cpp"
    break;

  case 46: /* exp: PEBL_LPAREN nlornone exp nlornone PEBL_RPAREN  */
#line 424 "src/base/grammar.y"
                                                                 {((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp);}
#line 1687 "src/base/grammar.tab.cpp"
    break;

  case 47: /* exp: PEBL_SUBTRACT exp  */
#line 427 "src/base/grammar.y"
                                                        {
		Variant tmpV = 0;
		PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);
		((*yyvalp).exp) = new OpNode(PEBL_SUBTRACT, tmpNode, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno); }
#line 1696 "src/base/grammar.tab.cpp"
    break;

  case 48: /* exp: PEBL_NOT exp  */
#line 433 "src/base/grammar.y"
                                                    {((*yyvalp).exp) = new OpNode(PEBL_NOT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), NULL, sourcefilename, yylineno); }
#line 1702 "src/base/grammar.tab.cpp"
    break;

  case 49: /* exp: exp PEBL_ADD nlornone exp  */
#line 436 "src/base/grammar.y"
                                                  { ((*yyvalp).exp) = new OpNode(PEBL_ADD, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);  }
#line 1708 "src/base/grammar.tab.cpp"
    break;

  case 50: /* exp: exp PEBL_DIVIDE nlornone exp  */
#line 440 "src/base/grammar.y"
                                                   { ((*yyvalp).exp) = new OpNode(PEBL_DIVIDE,(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);  }
#line 1714 "src/base/grammar.tab.cpp"
    break;

  case 51: /* exp: exp PEBL_MULTIPLY nlornone exp  */
#line 443 "src/base/grammar.y"
                                                  { ((*yyvalp).exp) = new OpNode(PEBL_MULTIPLY, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1720 "src/base/grammar.tab.cpp"
    break;

  case 52: /* exp: exp PEBL_POWER nlornone exp  */
#line 446 "src/base/grammar.y"
                                               { ((*yyvalp).exp) = new OpNode(PEBL_POWER, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1726 "src/base/grammar.tab.cpp"
    break;

  case 53: /* exp: exp PEBL_SUBTRACT nlornone exp  */
#line 449 "src/base/grammar.y"
                                                  { ((*yyvalp).exp) = new OpNode(PEBL_SUBTRACT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno); }
#line 1732 "src/base/grammar.tab.cpp"
    break;

  case 54: /* exp: exp PEBL_OR nlornone exp  */
#line 452 "src/base/grammar.y"
                                            { ((*yyvalp).exp) = new OpNode(PEBL_OR, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno); }
#line 1738 "src/base/grammar.tab.cpp"
    break;

  case 55: /* exp: exp PEBL_AND nlornone exp  */
#line 455 "src/base/grammar.y"
                                            { ((*yyvalp).exp) = new OpNode(PEBL_AND, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno); }
#line 1744 "src/base/grammar.tab.cpp"
    break;

  case 56: /* exp: exp PEBL_LT exp  */
#line 459 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new OpNode(PEBL_LT,  (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1750 "src/base/grammar.tab.cpp"
    break;

  case 57: /* exp: exp PEBL_GT exp  */
#line 462 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new OpNode(PEBL_GT,  (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1756 "src/base/grammar.tab.cpp"
    break;

  case 58: /* exp: exp PEBL_GE exp  */
#line 465 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new OpNode(PEBL_GE,  (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1762 "src/base/grammar.tab.cpp"
    break;

  case 59: /* exp: exp PEBL_LE exp  */
#line 468 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new OpNode(PEBL_LE,  (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1768 "src/base/grammar.tab.cpp"
    break;

  case 60: /* exp: exp PEBL_EQ exp  */
#line 471 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new OpNode(PEBL_EQ,  (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1774 "src/base/grammar.tab.cpp"
    break;

  case 61: /* exp: exp PEBL_NE exp  */
#line 474 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new OpNode(PEBL_NE,  (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval.exp), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);}
#line 1780 "src/base/grammar.tab.cpp"
    break;

  case 62: /* exp: PEBL_FUNCTIONNAME arglist  */
#line 483 "src/base/grammar.y"
                                          {


		  /*Memory leak here, by electricfence*/
		Variant v = Variant((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.function),P_DATA_FUNCTION);
		PNode * tmpNode = new DataNode(v, sourcefilename, yylineno);
		((*yyvalp).exp) = new OpNode(PEBL_FUNCTION, tmpNode, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp), sourcefilename, yylineno);
		free((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval.function));
		}
#line 1794 "src/base/grammar.tab.cpp"
    break;

  case 63: /* datum: PEBL_INTEGER  */
#line 497 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new DataNode ((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.iValue), sourcefilename, yylineno); }
#line 1800 "src/base/grammar.tab.cpp"
    break;

  case 64: /* datum: PEBL_FLOAT  */
#line 500 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = new DataNode ((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.fValue), sourcefilename, yylineno);}
#line 1806 "src/base/grammar.tab.cpp"
    break;

  case 65: /* datum: PEBL_STRING  */
#line 503 "src/base/grammar.y"
                                         {
	  Variant tmpV((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.strValue));            /*create a new temporary variant*/
	  free((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.strValue));
	  ((*yyvalp).exp) = new DataNode(tmpV, sourcefilename, yylineno);

                        }
#line 1817 "src/base/grammar.tab.cpp"
    break;

  case 66: /* datum: list  */
#line 510 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp);}
#line 1823 "src/base/grammar.tab.cpp"
    break;

  case 67: /* datum: variable  */
#line 513 "src/base/grammar.y"
                                         { ((*yyvalp).exp) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.exp);}
#line 1829 "src/base/grammar.tab.cpp"
    break;

  case 68: /* variable: PEBL_LOCALVAR  */
#line 518 "src/base/grammar.y"
                                           { 
		Variant tmpV((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.symbol), P_DATA_LOCALVARIABLE);           /*create a new temporary variant*/;
		((*yyvalp).exp) = new DataNode(tmpV, sourcefilename, yylineno);                        /*Make a new variable node here.*/
                free((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.symbol));
                }
#line 1839 "src/base/grammar.tab.cpp"
    break;

  case 69: /* variable: PEBL_GLOBALVAR  */
#line 523 "src/base/grammar.y"
                                            { 
		Variant tmpV((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.symbol), P_DATA_GLOBALVARIABLE);          /*create a new temporary variant*/;
		((*yyvalp).exp) = new DataNode(tmpV, sourcefilename, yylineno);  /*Make a new variable node here.*/
		free((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval.symbol));
		 }
#line 1849 "src/base/grammar.tab.cpp"
    break;

  case 70: /* nlornone: %empty  */
#line 532 "src/base/grammar.y"
                       {/*nothing*/;}
#line 1855 "src/base/grammar.tab.cpp"
    break;

  case 71: /* nlornone: newlines  */
#line 533 "src/base/grammar.y"
                                 {/**/;}
#line 1861 "src/base/grammar.tab.cpp"
    break;

  case 72: /* newlines: PEBL_NEWLINE  */
#line 537 "src/base/grammar.y"
                                      {/**/;}
#line 1867 "src/base/grammar.tab.cpp"
    break;

  case 73: /* newlines: newlines PEBL_NEWLINE  */
#line 538 "src/base/grammar.y"
                                      {/**/;}
#line 1873 "src/base/grammar.tab.cpp"
    break;


#line 1877 "src/base/grammar.tab.cpp"

      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yylhsNonterm (yyrule), yyvalp, yylocp);

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYNOMEM
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
}


static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YY_USE (yy0);
  YY_USE (yy1);

  switch (yyn)
    {

      default: break;
    }
}

                              /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void
yydestroyGLRState (char const *yymsg, yyGLRState *yys)
{
  if (yys->yyresolved)
    yydestruct (yymsg, yy_accessing_symbol (yys->yylrState),
                &yys->yysemantics.yyval);
  else
    {
#if YYDEBUG
      if (yydebug)
        {
          if (yys->yysemantics.yyfirstVal)
            YY_FPRINTF ((stderr, "%s unresolved", yymsg));
          else
            YY_FPRINTF ((stderr, "%s incomplete", yymsg));
          YY_SYMBOL_PRINT ("", yy_accessing_symbol (yys->yylrState), YY_NULLPTR, &yys->yyloc);
        }
#endif

      if (yys->yysemantics.yyfirstVal)
        {
          yySemanticOption *yyoption = yys->yysemantics.yyfirstVal;
          yyGLRState *yyrh;
          int yyn;
          for (yyrh = yyoption->yystate, yyn = yyrhsLength (yyoption->yyrule);
               yyn > 0;
               yyrh = yyrh->yypred, yyn -= 1)
            yydestroyGLRState (yymsg, yyrh);
        }
    }
}

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

/** True iff LR state YYSTATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yy_state_t yystate)
{
  return yypact_value_is_default (yypact[yystate]);
}

/** The default reduction for YYSTATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yy_state_t yystate)
{
  return yydefact[yystate];
}

#define yytable_value_is_error(Yyn) \
  0

/** The action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *YYCONFLICTS to a pointer into yyconfl to a 0-terminated list
 *  of conflicting reductions.
 */
static inline int
yygetLRActions (yy_state_t yystate, yysymbol_kind_t yytoken, const short** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yytoken == YYSYMBOL_YYerror)
    {
      // This is the error token.
      *yyconflicts = yyconfl;
      return 0;
    }
  else if (yyisDefaultedState (yystate)
           || yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyconflicts = yyconfl;
      return -yydefact[yystate];
    }
  else if (! yytable_value_is_error (yytable[yyindex]))
    {
      *yyconflicts = yyconfl + yyconflp[yyindex];
      return yytable[yyindex];
    }
  else
    {
      *yyconflicts = yyconfl + yyconflp[yyindex];
      return 0;
    }
}

/** Compute post-reduction state.
 * \param yystate   the current state
 * \param yysym     the nonterminal to push on the stack
 */
static inline yy_state_t
yyLRgotoState (yy_state_t yystate, yysymbol_kind_t yysym)
{
  int yyr = yypgoto[yysym - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yysym - YYNTOKENS];
}

static inline yybool
yyisShiftAction (int yyaction)
{
  return 0 < yyaction;
}

static inline yybool
yyisErrorAction (int yyaction)
{
  return yyaction == 0;
}

                                /* GLRStates */

/** Return a fresh GLRStackItem in YYSTACKP.  The item is an LR state
 *  if YYISSTATE, and otherwise a semantic option.  Callers should call
 *  YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 *  headroom.  */

static inline yyGLRStackItem*
yynewGLRStackItem (yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  YYRULE on the semantic values in YYRHS to the list of
 *  alternative actions for YYSTATE.  Assumes that YYRHS comes from
 *  stack #YYK of *YYSTACKP. */
static void
yyaddDeferredAction (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyGLRState* yystate,
                     yyGLRState* yyrhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption =
    &yynewGLRStackItem (yystackp, yyfalse)->yyoption;
  YY_ASSERT (!yynewOption->yyisState);
  yynewOption->yystate = yyrhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
    {
      yynewOption->yyrawchar = yychar;
      yynewOption->yyval = yylval;
    }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK (yystackp);
}

                                /* GLRStacks */

/** Initialize YYSET to a singleton set containing an empty stack.  */
static yybool
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates
    = YY_CAST (yyGLRState**,
               YYMALLOC (YY_CAST (YYSIZE_T, yyset->yycapacity)
                         * sizeof yyset->yystates[0]));
  if (! yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = YY_NULLPTR;
  yyset->yylookaheadNeeds
    = YY_CAST (yybool*,
               YYMALLOC (YY_CAST (YYSIZE_T, yyset->yycapacity)
                         * sizeof yyset->yylookaheadNeeds[0]));
  if (! yyset->yylookaheadNeeds)
    {
      YYFREE (yyset->yystates);
      return yyfalse;
    }
  memset (yyset->yylookaheadNeeds,
          0,
          YY_CAST (YYSIZE_T, yyset->yycapacity) * sizeof yyset->yylookaheadNeeds[0]);
  return yytrue;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
  YYFREE (yyset->yylookaheadNeeds);
}

/** Initialize *YYSTACKP to a single empty stack, with total maximum
 *  capacity for all stacks of YYSIZE.  */
static yybool
yyinitGLRStack (yyGLRStack* yystackp, YYPTRDIFF_T yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems
    = YY_CAST (yyGLRStackItem*,
               YYMALLOC (YY_CAST (YYSIZE_T, yysize)
                         * sizeof yystackp->yynextFree[0]));
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = YY_NULLPTR;
  yystackp->yylastDeleted = YY_NULLPTR;
  return yyinitStateSet (&yystackp->yytops);
}


#if YYSTACKEXPANDABLE
# define YYRELOC(YYFROMITEMS, YYTOITEMS, YYX, YYTYPE)                   \
  &((YYTOITEMS)                                                         \
    - ((YYFROMITEMS) - YY_REINTERPRET_CAST (yyGLRStackItem*, (YYX))))->YYTYPE

/** If *YYSTACKP is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void
yyexpandGLRStack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem* yyp0, *yyp1;
  YYPTRDIFF_T yynewSize;
  YYPTRDIFF_T yyn;
  YYPTRDIFF_T yysize = yystackp->yynextFree - yystackp->yyitems;
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted (yystackp);
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems
    = YY_CAST (yyGLRStackItem*,
               YYMALLOC (YY_CAST (YYSIZE_T, yynewSize)
                         * sizeof yynewItems[0]));
  if (! yynewItems)
    yyMemoryExhausted (yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize;
       0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
    {
      *yyp1 = *yyp0;
      if (*YY_REINTERPRET_CAST (yybool *, yyp0))
        {
          yyGLRState* yys0 = &yyp0->yystate;
          yyGLRState* yys1 = &yyp1->yystate;
          if (yys0->yypred != YY_NULLPTR)
            yys1->yypred =
              YYRELOC (yyp0, yyp1, yys0->yypred, yystate);
          if (! yys0->yyresolved && yys0->yysemantics.yyfirstVal != YY_NULLPTR)
            yys1->yysemantics.yyfirstVal =
              YYRELOC (yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
        }
      else
        {
          yySemanticOption* yyv0 = &yyp0->yyoption;
          yySemanticOption* yyv1 = &yyp1->yyoption;
          if (yyv0->yystate != YY_NULLPTR)
            yyv1->yystate = YYRELOC (yyp0, yyp1, yyv0->yystate, yystate);
          if (yyv0->yynext != YY_NULLPTR)
            yyv1->yynext = YYRELOC (yyp0, yyp1, yyv0->yynext, yyoption);
        }
    }
  if (yystackp->yysplitPoint != YY_NULLPTR)
    yystackp->yysplitPoint = YYRELOC (yystackp->yyitems, yynewItems,
                                      yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != YY_NULLPTR)
      yystackp->yytops.yystates[yyn] =
        YYRELOC (yystackp->yyitems, yynewItems,
                 yystackp->yytops.yystates[yyn], yystate);
  YYFREE (yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void
yyfreeGLRStack (yyGLRStack* yystackp)
{
  YYFREE (yystackp->yyitems);
  yyfreeStateSet (&yystackp->yytops);
}

/** Assuming that YYS is a GLRState somewhere on *YYSTACKP, update the
 *  splitpoint of *YYSTACKP, if needed, so that it is at least as deep as
 *  YYS.  */
static inline void
yyupdateSplit (yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != YY_NULLPTR && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #YYK in *YYSTACKP.  */
static inline void
yymarkStackDeleted (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  if (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = YY_NULLPTR;
}

/** Undelete the last stack in *YYSTACKP that was marked as deleted.  Can
    only be done once after a deletion, and only when all other stacks have
    been deleted.  */
static void
yyundeleteLastStack (yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == YY_NULLPTR || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YY_DPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = YY_NULLPTR;
}

static inline void
yyremoveDeletes (yyGLRStack* yystackp)
{
  YYPTRDIFF_T yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
    {
      if (yystackp->yytops.yystates[yyi] == YY_NULLPTR)
        {
          if (yyi == yyj)
            YY_DPRINTF ((stderr, "Removing dead stacks.\n"));
          yystackp->yytops.yysize -= 1;
        }
      else
        {
          yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
          /* In the current implementation, it's unnecessary to copy
             yystackp->yytops.yylookaheadNeeds[yyi] since, after
             yyremoveDeletes returns, the parser immediately either enters
             deterministic operation or shifts a token.  However, it doesn't
             hurt, and the code might evolve to need it.  */
          yystackp->yytops.yylookaheadNeeds[yyj] =
            yystackp->yytops.yylookaheadNeeds[yyi];
          if (yyj != yyi)
            YY_DPRINTF ((stderr, "Rename stack %ld -> %ld.\n",
                        YY_CAST (long, yyi), YY_CAST (long, yyj)));
          yyj += 1;
        }
      yyi += 1;
    }
}

/** Shift to a new state on stack #YYK of *YYSTACKP, corresponding to LR
 * state YYLRSTATE, at input position YYPOSN, with (resolved) semantic
 * value *YYVALP and source location *YYLOCP.  */
static inline void
yyglrShift (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yy_state_t yylrState,
            YYPTRDIFF_T yyposn,
            YYSTYPE* yyvalp)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyval = *yyvalp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK (yystackp);
}

/** Shift stack #YYK of *YYSTACKP, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static inline void
yyglrShiftDefer (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yy_state_t yylrState,
                 YYPTRDIFF_T yyposn, yyGLRState* yyrhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;
  YY_ASSERT (yynewState->yyisState);

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = YY_NULLPTR;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction (yystackp, yyk, yynewState, yyrhs, yyrule);
}

#if YYDEBUG

/*----------------------------------------------------------------------.
| Report that stack #YYK of *YYSTACKP is going to be reduced by YYRULE. |
`----------------------------------------------------------------------*/

static inline void
yy_reduce_print (yybool yynormal, yyGLRStackItem* yyvsp, YYPTRDIFF_T yyk,
                 yyRuleNum yyrule)
{
  int yynrhs = yyrhsLength (yyrule);
  int yyi;
  YY_FPRINTF ((stderr, "Reducing stack %ld by rule %d (line %d):\n",
               YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule]));
  if (! yynormal)
    yyfillin (yyvsp, 1, -yynrhs);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YY_FPRINTF ((stderr, "   $%d = ", yyi + 1));
      yy_symbol_print (stderr,
                       yy_accessing_symbol (yyvsp[yyi - yynrhs + 1].yystate.yylrState),
                       &yyvsp[yyi - yynrhs + 1].yystate.yysemantics.yyval                       );
      if (!yyvsp[yyi - yynrhs + 1].yystate.yyresolved)
        YY_FPRINTF ((stderr, " (unresolved)"));
      YY_FPRINTF ((stderr, "\n"));
    }
}
#endif

/** Pop the symbols consumed by reduction #YYRULE from the top of stack
 *  #YYK of *YYSTACKP, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *YYVALP to the resulting value,
 *  and *YYLOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyRuleNum yyrule,
            YYSTYPE* yyvalp)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystackp->yysplitPoint == YY_NULLPTR)
    {
      /* Standard special case: single stack.  */
      yyGLRStackItem* yyrhs
        = YY_REINTERPRET_CAST (yyGLRStackItem*, yystackp->yytops.yystates[yyk]);
      YY_ASSERT (yyk == 0);
      yystackp->yynextFree -= yynrhs;
      yystackp->yyspaceLeft += yynrhs;
      yystackp->yytops.yystates[0] = & yystackp->yynextFree[-1].yystate;
      return yyuserAction (yyrule, yynrhs, yyrhs, yystackp, yyk,
                           yyvalp);
    }
  else
    {
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yyGLRState* yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
        = yystackp->yytops.yystates[yyk];
      int yyi;
      for (yyi = 0; yyi < yynrhs; yyi += 1)
        {
          yys = yys->yypred;
          YY_ASSERT (yys);
        }
      yyupdateSplit (yystackp, yys);
      yystackp->yytops.yystates[yyk] = yys;
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
                           yystackp, yyk, yyvalp);
    }
}

/** Pop items off stack #YYK of *YYSTACKP according to grammar rule YYRULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with YYRULE and store its value with the
 *  newly pushed state, if YYFORCEEVAL or if *YYSTACKP is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #YYK from
 *  *YYSTACKP.  In this case, the semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyRuleNum yyrule,
             yybool yyforceEval)
{
  YYPTRDIFF_T yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == YY_NULLPTR)
    {
      YYSTYPE yyval;

      YYRESULTTAG yyflag = yydoAction (yystackp, yyk, yyrule, &yyval);
      if (yyflag == yyerr && yystackp->yysplitPoint != YY_NULLPTR)
        YY_DPRINTF ((stderr,
                     "Parse on stack %ld rejected by rule %d (line %d).\n",
                     YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule]));
      if (yyflag != yyok)
        return yyflag;
      yyglrShift (yystackp, yyk,
                  yyLRgotoState (yystackp->yytops.yystates[yyk]->yylrState,
                                 yylhsNonterm (yyrule)),
                  yyposn, &yyval);
    }
  else
    {
      YYPTRDIFF_T yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystackp->yytops.yystates[yyk];
      yy_state_t yynewLRState;

      for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
           0 < yyn; yyn -= 1)
        {
          yys = yys->yypred;
          YY_ASSERT (yys);
        }
      yyupdateSplit (yystackp, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YY_DPRINTF ((stderr,
                   "Reduced stack %ld by rule %d (line %d); action deferred.  "
                   "Now in state %d.\n",
                   YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule],
                   yynewLRState));
      for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
        if (yyi != yyk && yystackp->yytops.yystates[yyi] != YY_NULLPTR)
          {
            yyGLRState *yysplit = yystackp->yysplitPoint;
            yyGLRState *yyp = yystackp->yytops.yystates[yyi];
            while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
              {
                if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
                  {
                    yyaddDeferredAction (yystackp, yyk, yyp, yys0, yyrule);
                    yymarkStackDeleted (yystackp, yyk);
                    YY_DPRINTF ((stderr, "Merging stack %ld into stack %ld.\n",
                                 YY_CAST (long, yyk), YY_CAST (long, yyi)));
                    return yyok;
                  }
                yyp = yyp->yypred;
              }
          }
      yystackp->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static YYPTRDIFF_T
yysplitStack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  if (yystackp->yysplitPoint == YY_NULLPTR)
    {
      YY_ASSERT (yyk == 0);
      yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
    }
  if (yystackp->yytops.yycapacity <= yystackp->yytops.yysize)
    {
      YYPTRDIFF_T state_size = YYSIZEOF (yystackp->yytops.yystates[0]);
      YYPTRDIFF_T half_max_capacity = YYSIZE_MAXIMUM / 2 / state_size;
      if (half_max_capacity < yystackp->yytops.yycapacity)
        yyMemoryExhausted (yystackp);
      yystackp->yytops.yycapacity *= 2;

      {
        yyGLRState** yynewStates
          = YY_CAST (yyGLRState**,
                     YYREALLOC (yystackp->yytops.yystates,
                                (YY_CAST (YYSIZE_T, yystackp->yytops.yycapacity)
                                 * sizeof yynewStates[0])));
        if (yynewStates == YY_NULLPTR)
          yyMemoryExhausted (yystackp);
        yystackp->yytops.yystates = yynewStates;
      }

      {
        yybool* yynewLookaheadNeeds
          = YY_CAST (yybool*,
                     YYREALLOC (yystackp->yytops.yylookaheadNeeds,
                                (YY_CAST (YYSIZE_T, yystackp->yytops.yycapacity)
                                 * sizeof yynewLookaheadNeeds[0])));
        if (yynewLookaheadNeeds == YY_NULLPTR)
          yyMemoryExhausted (yystackp);
        yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
      }
    }
  yystackp->yytops.yystates[yystackp->yytops.yysize]
    = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize]
    = yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize - 1;
}

/** True iff YYY0 and YYY1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
static yybool
yyidenticalOptions (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
    {
      yyGLRState *yys0, *yys1;
      int yyn;
      for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
           yyn = yyrhsLength (yyy0->yyrule);
           yyn > 0;
           yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
        if (yys0->yyposn != yys1->yyposn)
          return yyfalse;
      return yytrue;
    }
  else
    return yyfalse;
}

/** Assuming identicalOptions (YYY0,YYY1), destructively merge the
 *  alternative semantic values for the RHS-symbols of YYY1 and YYY0.  */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       0 < yyn;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    {
      if (yys0 == yys1)
        break;
      else if (yys0->yyresolved)
        {
          yys1->yyresolved = yytrue;
          yys1->yysemantics.yyval = yys0->yysemantics.yyval;
        }
      else if (yys1->yyresolved)
        {
          yys0->yyresolved = yytrue;
          yys0->yysemantics.yyval = yys1->yysemantics.yyval;
        }
      else
        {
          yySemanticOption** yyz0p = &yys0->yysemantics.yyfirstVal;
          yySemanticOption* yyz1 = yys1->yysemantics.yyfirstVal;
          while (yytrue)
            {
              if (yyz1 == *yyz0p || yyz1 == YY_NULLPTR)
                break;
              else if (*yyz0p == YY_NULLPTR)
                {
                  *yyz0p = yyz1;
                  break;
                }
              else if (*yyz0p < yyz1)
                {
                  yySemanticOption* yyz = *yyz0p;
                  *yyz0p = yyz1;
                  yyz1 = yyz1->yynext;
                  (*yyz0p)->yynext = yyz;
                }
              yyz0p = &(*yyz0p)->yynext;
            }
          yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
        }
    }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
static int
yypreference (yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
    {
      if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
        return 0;
      else
        return 1;
    }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp);


/** Resolve the previous YYN states starting at and including state YYS
 *  on *YYSTACKP. If result != yyok, some states may have been left
 *  unresolved possibly with empty semantic option chains.  Regardless
 *  of whether result = yyok, each state has been left with consistent
 *  data so that yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn,
                 yyGLRStack* yystackp)
{
  if (0 < yyn)
    {
      YY_ASSERT (yys->yypred);
      YYCHK (yyresolveStates (yys->yypred, yyn-1, yystackp));
      if (! yys->yyresolved)
        YYCHK (yyresolveValue (yys, yystackp));
    }
  return yyok;
}

/** Resolve the states for the RHS of YYOPT on *YYSTACKP, perform its
 *  user action, and return the semantic value and location in *YYVALP
 *  and *YYLOCP.  Regardless of whether result = yyok, all RHS states
 *  have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystackp,
                 YYSTYPE* yyvalp)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs = yyrhsLength (yyopt->yyrule);
  YYRESULTTAG yyflag =
    yyresolveStates (yyopt->yystate, yynrhs, yystackp);
  if (yyflag != yyok)
    {
      yyGLRState *yys;
      for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
        yydestroyGLRState ("Cleanup: popping", yys);
      return yyflag;
    }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  {
    int yychar_current = yychar;
    YYSTYPE yylval_current = yylval;
    yychar = yyopt->yyrawchar;
    yylval = yyopt->yyval;
    yyflag = yyuserAction (yyopt->yyrule, yynrhs,
                           yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
                           yystackp, -1, yyvalp);
    yychar = yychar_current;
    yylval = yylval_current;
  }
  return yyflag;
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == YY_NULLPTR)
    {
      yyleftmost_state.yyposn = 0;
      yystates[0] = &yyleftmost_state;
    }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YY_FPRINTF ((stderr, "%*s%s -> <Rule %d, empty>\n",
                 yyindent, "", yysymbol_name (yylhsNonterm (yyx->yyrule)),
                 yyx->yyrule - 1));
  else
    YY_FPRINTF ((stderr, "%*s%s -> <Rule %d, tokens %ld .. %ld>\n",
                 yyindent, "", yysymbol_name (yylhsNonterm (yyx->yyrule)),
                 yyx->yyrule - 1, YY_CAST (long, yys->yyposn + 1),
                 YY_CAST (long, yyx->yystate->yyposn)));
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
    {
      if (yystates[yyi]->yyresolved)
        {
          if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
            YY_FPRINTF ((stderr, "%*s%s <empty>\n", yyindent+2, "",
                         yysymbol_name (yy_accessing_symbol (yystates[yyi]->yylrState))));
          else
            YY_FPRINTF ((stderr, "%*s%s <tokens %ld .. %ld>\n", yyindent+2, "",
                         yysymbol_name (yy_accessing_symbol (yystates[yyi]->yylrState)),
                         YY_CAST (long, yystates[yyi-1]->yyposn + 1),
                         YY_CAST (long, yystates[yyi]->yyposn)));
        }
      else
        yyreportTree (yystates[yyi]->yysemantics.yyfirstVal, yyindent+2);
    }
}
#endif

static YYRESULTTAG
yyreportAmbiguity (yySemanticOption* yyx0,
                   yySemanticOption* yyx1)
{
  YY_USE (yyx0);
  YY_USE (yyx1);

#if YYDEBUG
  YY_FPRINTF ((stderr, "Ambiguity detected.\n"));
  YY_FPRINTF ((stderr, "Option 1,\n"));
  yyreportTree (yyx0, 2);
  YY_FPRINTF ((stderr, "\nOption 2,\n"));
  yyreportTree (yyx1, 2);
  YY_FPRINTF ((stderr, "\n"));
#endif

  yyerror (YY_("syntax is ambiguous"));
  return yyabort;
}

/** Resolve the ambiguity represented in state YYS in *YYSTACKP,
 *  perform the indicated actions, and set the semantic value of YYS.
 *  If result != yyok, the chain of semantic options in YYS has been
 *  cleared instead or it has been left unmodified except that
 *  redundant options may have been removed.  Regardless of whether
 *  result = yyok, YYS has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest = yyoptionList;
  yySemanticOption** yypp;
  yybool yymerge = yyfalse;
  YYSTYPE yyval;
  YYRESULTTAG yyflag;

  for (yypp = &yyoptionList->yynext; *yypp != YY_NULLPTR; )
    {
      yySemanticOption* yyp = *yypp;

      if (yyidenticalOptions (yybest, yyp))
        {
          yymergeOptionSets (yybest, yyp);
          *yypp = yyp->yynext;
        }
      else
        {
          switch (yypreference (yybest, yyp))
            {
            case 0:
              return yyreportAmbiguity (yybest, yyp);
              break;
            case 1:
              yymerge = yytrue;
              break;
            case 2:
              break;
            case 3:
              yybest = yyp;
              yymerge = yyfalse;
              break;
            default:
              /* This cannot happen so it is not worth a YY_ASSERT (yyfalse),
                 but some compilers complain if the default case is
                 omitted.  */
              break;
            }
          yypp = &yyp->yynext;
        }
    }

  if (yymerge)
    {
      yySemanticOption* yyp;
      int yyprec = yydprec[yybest->yyrule];
      yyflag = yyresolveAction (yybest, yystackp, &yyval);
      if (yyflag == yyok)
        for (yyp = yybest->yynext; yyp != YY_NULLPTR; yyp = yyp->yynext)
          {
            if (yyprec == yydprec[yyp->yyrule])
              {
                YYSTYPE yyval_other;
                yyflag = yyresolveAction (yyp, yystackp, &yyval_other);
                if (yyflag != yyok)
                  {
                    yydestruct ("Cleanup: discarding incompletely merged value for",
                                yy_accessing_symbol (yys->yylrState),
                                &yyval);
                    break;
                  }
                yyuserMerge (yymerger[yyp->yyrule], &yyval, &yyval_other);
              }
          }
    }
  else
    yyflag = yyresolveAction (yybest, yystackp, &yyval);

  if (yyflag == yyok)
    {
      yys->yyresolved = yytrue;
      yys->yysemantics.yyval = yyval;
    }
  else
    yys->yysemantics.yyfirstVal = YY_NULLPTR;
  return yyflag;
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystackp)
{
  if (yystackp->yysplitPoint != YY_NULLPTR)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystackp->yytops.yystates[0];
           yys != yystackp->yysplitPoint;
           yys = yys->yypred, yyn += 1)
        continue;
      YYCHK (yyresolveStates (yystackp->yytops.yystates[0], yyn, yystackp
                             ));
    }
  return yyok;
}

/** Called when returning to deterministic operation to clean up the extra
 * stacks. */
static void
yycompressStack (yyGLRStack* yystackp)
{
  /* yyr is the state after the split point.  */
  yyGLRState *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == YY_NULLPTR)
    return;

  {
    yyGLRState *yyp, *yyq;
    for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = YY_NULLPTR;
         yyp != yystackp->yysplitPoint;
         yyr = yyp, yyp = yyq, yyq = yyp->yypred)
      yyp->yypred = yyr;
  }

  yystackp->yyspaceLeft += yystackp->yynextFree - yystackp->yyitems;
  yystackp->yynextFree = YY_REINTERPRET_CAST (yyGLRStackItem*, yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= yystackp->yynextFree - yystackp->yyitems;
  yystackp->yysplitPoint = YY_NULLPTR;
  yystackp->yylastDeleted = YY_NULLPTR;

  while (yyr != YY_NULLPTR)
    {
      yystackp->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
      yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
      yystackp->yynextFree += 1;
      yystackp->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystackp, YYPTRDIFF_T yyk,
                   YYPTRDIFF_T yyposn)
{
  while (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
    {
      yy_state_t yystate = yystackp->yytops.yystates[yyk]->yylrState;
      YY_DPRINTF ((stderr, "Stack %ld Entering state %d\n",
                   YY_CAST (long, yyk), yystate));

      YY_ASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
        {
          YYRESULTTAG yyflag;
          yyRuleNum yyrule = yydefaultAction (yystate);
          if (yyrule == 0)
            {
              YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              return yyok;
            }
          yyflag = yyglrReduce (yystackp, yyk, yyrule, yyimmediate[yyrule]);
          if (yyflag == yyerr)
            {
              YY_DPRINTF ((stderr,
                           "Stack %ld dies "
                           "(predicate failure or explicit user error).\n",
                           YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              return yyok;
            }
          if (yyflag != yyok)
            return yyflag;
        }
      else
        {
          yysymbol_kind_t yytoken = yygetToken (&yychar);
          const short* yyconflicts;
          const int yyaction = yygetLRActions (yystate, yytoken, &yyconflicts);
          yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;

          for (/* nothing */; *yyconflicts; yyconflicts += 1)
            {
              YYRESULTTAG yyflag;
              YYPTRDIFF_T yynewStack = yysplitStack (yystackp, yyk);
              YY_DPRINTF ((stderr, "Splitting off stack %ld from %ld.\n",
                           YY_CAST (long, yynewStack), YY_CAST (long, yyk)));
              yyflag = yyglrReduce (yystackp, yynewStack,
                                    *yyconflicts,
                                    yyimmediate[*yyconflicts]);
              if (yyflag == yyok)
                YYCHK (yyprocessOneStack (yystackp, yynewStack,
                                          yyposn));
              else if (yyflag == yyerr)
                {
                  YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yynewStack)));
                  yymarkStackDeleted (yystackp, yynewStack);
                }
              else
                return yyflag;
            }

          if (yyisShiftAction (yyaction))
            break;
          else if (yyisErrorAction (yyaction))
            {
              YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              break;
            }
          else
            {
              YYRESULTTAG yyflag = yyglrReduce (yystackp, yyk, -yyaction,
                                                yyimmediate[-yyaction]);
              if (yyflag == yyerr)
                {
                  YY_DPRINTF ((stderr,
                               "Stack %ld dies "
                               "(predicate failure or explicit user error).\n",
                               YY_CAST (long, yyk)));
                  yymarkStackDeleted (yystackp, yyk);
                  break;
                }
              else if (yyflag != yyok)
                return yyflag;
            }
        }
    }
  return yyok;
}






static void
yyreportSyntaxError (yyGLRStack* yystackp)
{
  if (yystackp->yyerrState != 0)
    return;
  yyerror (YY_("syntax error"));
  yynerrs += 1;
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the lookahead.  */
static void
yyrecoverSyntaxError (yyGLRStack* yystackp)
{
  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (yytrue)
      {
        yysymbol_kind_t yytoken;
        int yyj;
        if (yychar == YYEOF)
          yyFail (yystackp, YY_NULLPTR);
        if (yychar != YYEMPTY)
          {
            yytoken = YYTRANSLATE (yychar);
            yydestruct ("Error: discarding",
                        yytoken, &yylval);
            yychar = YYEMPTY;
          }
        yytoken = yygetToken (&yychar);
        yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
        if (yypact_value_is_default (yyj))
          return;
        yyj += yytoken;
        if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
          {
            if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
              return;
          }
        else if (! yytable_value_is_error (yytable[yyj]))
          return;
      }

  /* Reduce to one stack.  */
  {
    YYPTRDIFF_T yyk;
    for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
      if (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
        break;
    if (yyk >= yystackp->yytops.yysize)
      yyFail (yystackp, YY_NULLPTR);
    for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
      yymarkStackDeleted (yystackp, yyk);
    yyremoveDeletes (yystackp);
    yycompressStack (yystackp);
  }

  /* Pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != YY_NULLPTR)
    {
      yyGLRState *yys = yystackp->yytops.yystates[0];
      int yyj = yypact[yys->yylrState];
      if (! yypact_value_is_default (yyj))
        {
          yyj += YYSYMBOL_YYerror;
          if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYSYMBOL_YYerror
              && yyisShiftAction (yytable[yyj]))
            {
              /* Shift the error token.  */
              int yyaction = yytable[yyj];
              YY_SYMBOL_PRINT ("Shifting", yy_accessing_symbol (yyaction),
                               &yylval, &yyerrloc);
              yyglrShift (yystackp, 0, yyaction,
                          yys->yyposn, &yylval);
              yys = yystackp->yytops.yystates[0];
              break;
            }
        }
      if (yys->yypred != YY_NULLPTR)
        yydestroyGLRState ("Error: popping", yys);
      yystackp->yytops.yystates[0] = yys->yypred;
      yystackp->yynextFree -= 1;
      yystackp->yyspaceLeft += 1;
    }
  if (yystackp->yytops.yystates[0] == YY_NULLPTR)
    yyFail (yystackp, YY_NULLPTR);
}

#define YYCHK1(YYE)                             \
  do {                                          \
    switch (YYE) {                              \
    case yyok:     break;                       \
    case yyabort:  goto yyabortlab;             \
    case yyaccept: goto yyacceptlab;            \
    case yyerr:    goto yyuser_error;           \
    case yynomem:  goto yyexhaustedlab;         \
    default:       goto yybuglab;               \
    }                                           \
  } while (0)

/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  YYPTRDIFF_T yyposn;

  YY_DPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;

  if (! yyinitGLRStack (yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP (yystack.yyexception_buffer))
    {
    case 0: break;
    case 1: goto yyabortlab;
    case 2: goto yyexhaustedlab;
    default: goto yybuglab;
    }
  yyglrShift (&yystack, 0, 0, 0, &yylval);
  yyposn = 0;

  while (yytrue)
    {
      /* For efficiency, we have two loops, the first of which is
         specialized to deterministic operation (single stack, no
         potential ambiguity).  */
      /* Standard mode. */
      while (yytrue)
        {
          yy_state_t yystate = yystack.yytops.yystates[0]->yylrState;
          YY_DPRINTF ((stderr, "Entering state %d\n", yystate));
          if (yystate == YYFINAL)
            goto yyacceptlab;
          if (yyisDefaultedState (yystate))
            {
              yyRuleNum yyrule = yydefaultAction (yystate);
              if (yyrule == 0)
                {
                  yyreportSyntaxError (&yystack);
                  goto yyuser_error;
                }
              YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue));
            }
          else
            {
              yysymbol_kind_t yytoken = yygetToken (&yychar);
              const short* yyconflicts;
              int yyaction = yygetLRActions (yystate, yytoken, &yyconflicts);
              if (*yyconflicts)
                /* Enter nondeterministic mode.  */
                break;
              if (yyisShiftAction (yyaction))
                {
                  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
                  yychar = YYEMPTY;
                  yyposn += 1;
                  yyglrShift (&yystack, 0, yyaction, yyposn, &yylval);
                  if (0 < yystack.yyerrState)
                    yystack.yyerrState -= 1;
                }
              else if (yyisErrorAction (yyaction))
                {
                  /* Issue an error message unless the scanner already
                     did. */
                  if (yychar != YYerror)
                    yyreportSyntaxError (&yystack);
                  goto yyuser_error;
                }
              else
                YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue));
            }
        }

      /* Nondeterministic mode. */
      while (yytrue)
        {
          yysymbol_kind_t yytoken_to_shift;
          YYPTRDIFF_T yys;

          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            yystackp->yytops.yylookaheadNeeds[yys] = yychar != YYEMPTY;

          /* yyprocessOneStack returns one of three things:

              - An error flag.  If the caller is yyprocessOneStack, it
                immediately returns as well.  When the caller is finally
                yyparse, it jumps to an error label via YYCHK1.

              - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
                (&yystack, yys), which sets the top state of yys to NULL.  Thus,
                yyparse's following invocation of yyremoveDeletes will remove
                the stack.

              - yyok, when ready to shift a token.

             Except in the first case, yyparse will invoke yyremoveDeletes and
             then shift the next token onto all remaining stacks.  This
             synchronization of the shift (that is, after all preceding
             reductions on all stacks) helps prevent double destructor calls
             on yylval in the event of memory exhaustion.  */

          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn));
          yyremoveDeletes (&yystack);
          if (yystack.yytops.yysize == 0)
            {
              yyundeleteLastStack (&yystack);
              if (yystack.yytops.yysize == 0)
                yyFail (&yystack, YY_("syntax error"));
              YYCHK1 (yyresolveStack (&yystack));
              YY_DPRINTF ((stderr, "Returning to deterministic operation.\n"));
              yyreportSyntaxError (&yystack);
              goto yyuser_error;
            }

          /* If any yyglrShift call fails, it will fail after shifting.  Thus,
             a copy of yylval will already be on stack 0 in the event of a
             failure in the following loop.  Thus, yychar is set to YYEMPTY
             before the loop to make sure the user destructor for yylval isn't
             called twice.  */
          yytoken_to_shift = YYTRANSLATE (yychar);
          yychar = YYEMPTY;
          yyposn += 1;
          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            {
              yy_state_t yystate = yystack.yytops.yystates[yys]->yylrState;
              const short* yyconflicts;
              int yyaction = yygetLRActions (yystate, yytoken_to_shift,
                              &yyconflicts);
              /* Note that yyconflicts were handled by yyprocessOneStack.  */
              YY_DPRINTF ((stderr, "On stack %ld, ", YY_CAST (long, yys)));
              YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, &yylval, &yylloc);
              yyglrShift (&yystack, yys, yyaction, yyposn,
                          &yylval);
              YY_DPRINTF ((stderr, "Stack %ld now in state %d\n",
                           YY_CAST (long, yys),
                           yystack.yytops.yystates[yys]->yylrState));
            }

          if (yystack.yytops.yysize == 1)
            {
              YYCHK1 (yyresolveStack (&yystack));
              YY_DPRINTF ((stderr, "Returning to deterministic operation.\n"));
              yycompressStack (&yystack);
              break;
            }
        }
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }

 yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;

 yybuglab:
  YY_ASSERT (yyfalse);
  goto yyabortlab;

 yyabortlab:
  yyresult = 1;
  goto yyreturnlab;

 yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;

 yyreturnlab:
  if (yychar != YYEMPTY)
    yydestruct ("Cleanup: discarding lookahead",
                YYTRANSLATE (yychar), &yylval);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
    {
      yyGLRState** yystates = yystack.yytops.yystates;
      if (yystates)
        {
          YYPTRDIFF_T yysize = yystack.yytops.yysize;
          YYPTRDIFF_T yyk;
          for (yyk = 0; yyk < yysize; yyk += 1)
            if (yystates[yyk])
              {
                while (yystates[yyk])
                  {
                    yyGLRState *yys = yystates[yyk];
                    if (yys->yypred != YY_NULLPTR)
                      yydestroyGLRState ("Cleanup: popping", yys);
                    yystates[yyk] = yys->yypred;
                    yystack.yynextFree -= 1;
                    yystack.yyspaceLeft += 1;
                  }
                break;
              }
        }
      yyfreeGLRStack (&yystack);
    }

  return yyresult;
}

/* DEBUGGING ONLY */
#if YYDEBUG
/* Print *YYS and its predecessors. */
static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      YY_FPRINTF ((stderr, " -> "));
    }
  YY_FPRINTF ((stderr, "%d@%ld", yys->yylrState, YY_CAST (long, yys->yyposn)));
}

/* Print YYS (possibly NULL) and its predecessors. */
static void
yypstates (yyGLRState* yys)
{
  if (yys == YY_NULLPTR)
    YY_FPRINTF ((stderr, "<null>"));
  else
    yy_yypstack (yys);
  YY_FPRINTF ((stderr, "\n"));
}

/* Print the stack #YYK.  */
static void
yypstack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  yypstates (yystackp->yytops.yystates[yyk]);
}

/* Print all the stacks.  */
static void
yypdumpstack (yyGLRStack* yystackp)
{
#define YYINDEX(YYX)                                                    \
  YY_CAST (long,                                                        \
           ((YYX)                                                       \
            ? YY_REINTERPRET_CAST (yyGLRStackItem*, (YYX)) - yystackp->yyitems \
            : -1))

  yyGLRStackItem* yyp;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
    {
      YY_FPRINTF ((stderr, "%3ld. ",
                   YY_CAST (long, yyp - yystackp->yyitems)));
      if (*YY_REINTERPRET_CAST (yybool *, yyp))
        {
          YY_ASSERT (yyp->yystate.yyisState);
          YY_ASSERT (yyp->yyoption.yyisState);
          YY_FPRINTF ((stderr, "Res: %d, LR State: %d, posn: %ld, pred: %ld",
                       yyp->yystate.yyresolved, yyp->yystate.yylrState,
                       YY_CAST (long, yyp->yystate.yyposn),
                       YYINDEX (yyp->yystate.yypred)));
          if (! yyp->yystate.yyresolved)
            YY_FPRINTF ((stderr, ", firstVal: %ld",
                         YYINDEX (yyp->yystate.yysemantics.yyfirstVal)));
        }
      else
        {
          YY_ASSERT (!yyp->yystate.yyisState);
          YY_ASSERT (!yyp->yyoption.yyisState);
          YY_FPRINTF ((stderr, "Option. rule: %d, state: %ld, next: %ld",
                       yyp->yyoption.yyrule - 1,
                       YYINDEX (yyp->yyoption.yystate),
                       YYINDEX (yyp->yyoption.yynext)));
        }
      YY_FPRINTF ((stderr, "\n"));
    }

  YY_FPRINTF ((stderr, "Tops:"));
  {
    YYPTRDIFF_T yyi;
    for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
      YY_FPRINTF ((stderr, "%ld: %ld; ", YY_CAST (long, yyi),
                   YYINDEX (yystackp->yytops.yystates[yyi])));
    YY_FPRINTF ((stderr, "\n"));
  }
#undef YYINDEX
}
#endif

#undef yylval
#undef yychar
#undef yynerrs




#line 544 "src/base/grammar.y"


void yyerror (const char *error)
{

  std::cerr << "line " << yylineno << " of "<< sourcefilename<<": " << error << std::endl;
  exit(1);
}

extern int yy_flex_debug;

PNode * parse (const char* filename)
{
  //yydebug=1;          /*Bison must be run with -t option to use debugging*/
  //yy_flex_debug = 1;   /*Flex must be run with -d option to use*/
  /*This is currently only called once, so use
   *it to open a hard-coded file for testing
   */
  
  ///Reset the linecounter
  yylineno = 1;

    FILE * filein = fopen(filename,"r");
    if(!filein) 
	{
	    fprintf(stderr,"Could not open %s\n",filename);
	    exit(1);
	}
    fprintf(stderr,"File [%s] opened successfully.\n", filename);
    yyin=filein;
    
    //Set global filename indicator to current filename for use in error reporting.
    sourcefilename = strdup(filename);

    yyparse ();

    free(sourcefilename);
    return gParseTreeHead;
}
