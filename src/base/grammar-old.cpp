/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




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
  void yyerror (char *error);
  int  yylex ();
  extern FILE * yyin;
  PNode * gParseTreeHead;
  PNode * parse(const char* filename);
  extern int yylineno;
  char* sourcefilename;



#line 101 "src/base/grammar.tab.cpp"

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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_SRC_BASE_GRAMMAR_TAB_HPP_INCLUDED
# define YY_YY_SRC_BASE_GRAMMAR_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    PEBL_AND = 258,
    PEBL_ADD = 259,
    PEBL_ARGLIST = 260,
    PEBL_ASSIGN = 261,
    PEBL_BREAK = 262,
    PEBL_COMMA = 263,
    PEBL_DEFINE = 264,
    PEBL_DIVIDE = 265,
    PEBL_DOT = 266,
    PEBL_ELSE = 267,
    PEBL_ELSEIF = 268,
    PEBL_END = 269,
    PEBL_EOF = 270,
    PEBL_EQ = 271,
    PEBL_FUNCTION = 272,
    PEBL_FUNCTIONS = 273,
    PEBL_GE = 274,
    PEBL_GT = 275,
    PEBL_IF = 276,
    PEBL_IFELSE = 277,
    PEBL_LAMBDAFUNCTION = 278,
    PEBL_LBRACE = 279,
    PEBL_LBRACKET = 280,
    PEBL_LE = 281,
    PEBL_LIBRARYFUNCTION = 282,
    PEBL_LISTHEAD = 283,
    PEBL_LISTITEM = 284,
    PEBL_LOOP = 285,
    PEBL_LPAREN = 286,
    PEBL_LT = 287,
    PEBL_MULTIPLY = 288,
    PEBL_NE = 289,
    PEBL_NEWLINE = 290,
    PEBL_NOT = 291,
    PEBL_OR = 292,
    PEBL_POWER = 293,
    PEBL_RETURN = 294,
    PEBL_RBRACE = 295,
    PEBL_RBRACKET = 296,
    PEBL_RPAREN = 297,
    PEBL_SUBTRACT = 298,
    PEBL_SEMI = 299,
    PEBL_STATEMENTS = 300,
    PEBL_START = 301,
    PEBL_VARPAIR = 302,
    PEBL_VARIABLEDATUM = 303,
    PEBL_VARLIST = 304,
    PEBL_WHILE = 305,
    PEBL_AND_TAIL = 306,
    PEBL_ADD_TAIL = 307,
    PEBL_ASSIGN_TAIL = 308,
    PEBL_BREAK_TAIL = 309,
    PEBL_COLON = 310,
    PEBL_DIVIDE_TAIL = 311,
    PEBL_EQ_TAIL = 312,
    PEBL_GE_TAIL = 313,
    PEBL_GT_TAIL = 314,
    PEBL_IF_TAIL = 315,
    PEBL_LE_TAIL = 316,
    PEBL_LISTITEM_TAIL = 317,
    PEBL_LOOP_TAIL1 = 318,
    PEBL_LOOP_TAIL2 = 319,
    PEBL_LT_TAIL = 320,
    PEBL_MULTIPLY_TAIL = 321,
    PEBL_NE_TAIL = 322,
    PEBL_NOT_TAIL = 323,
    PEBL_OR_TAIL = 324,
    PEBL_POWER_TAIL = 325,
    PEBL_RETURN_TAIL = 326,
    PEBL_SUBTRACT_TAIL = 327,
    PEBL_STATEMENTS_TAIL1 = 328,
    PEBL_STATEMENTS_TAIL2 = 329,
    PEBL_WHILE_TAIL = 330,
    PEBL_WHILE_TAIL2 = 331,
    PEBL_FUNCTION_TAIL1 = 332,
    PEBL_FUNCTION_TAIL2 = 333,
    PEBL_FUNCTION_TAIL_LIBFUNCTION = 334,
    PEBL_FLOAT = 335,
    PEBL_INTEGER = 336,
    PEBL_STRING = 337,
    PEBL_SYMBOL = 338,
    PEBL_LOCALVAR = 339,
    PEBL_GLOBALVAR = 340,
    PEBL_FUNCTIONNAME = 341,
    PEBL_UMINUS = 342
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 36 "src/base/grammar.y"

  
  pInt    iValue;  /* For the lexical analyser. NUMBER tokens */
  pDouble fValue;
  char        *strValue; 
  PNode       *exp;    /* For expressions. */
  char        *symbol; /* The name of a variable*/
  char        *function; 

#line 251 "src/base/grammar.tab.cpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_BASE_GRAMMAR_TAB_HPP_INCLUDED  */



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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   510

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  88
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  163

#define YYUNDEFTOK  2
#define YYMAXUTOK   342


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

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
      85,    86,    87
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   161,   161,   169,   177,   187,   202,   211,   215,   216,
     220,   221,   222,   224,   230,   233,   237,   241,   250,   252,
     254,   263,   271,   284,   290,   306,   313,   318,   325,   331,
     348,   352,   362,   365,   369,   373,   381,   385,   392,   394,
     402,   405,   408,   414,   417,   421,   424,   427,   430,   433,
     436,   440,   443,   446,   449,   452,   455,   464,   478,   481,
     484,   491,   494,   499,   504,   513,   514,   518,   519
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PEBL_AND", "PEBL_ADD", "PEBL_ARGLIST",
  "PEBL_ASSIGN", "PEBL_BREAK", "PEBL_COMMA", "PEBL_DEFINE", "PEBL_DIVIDE",
  "PEBL_DOT", "PEBL_ELSE", "PEBL_ELSEIF", "PEBL_END", "PEBL_EOF",
  "PEBL_EQ", "PEBL_FUNCTION", "PEBL_FUNCTIONS", "PEBL_GE", "PEBL_GT",
  "PEBL_IF", "PEBL_IFELSE", "PEBL_LAMBDAFUNCTION", "PEBL_LBRACE",
  "PEBL_LBRACKET", "PEBL_LE", "PEBL_LIBRARYFUNCTION", "PEBL_LISTHEAD",
  "PEBL_LISTITEM", "PEBL_LOOP", "PEBL_LPAREN", "PEBL_LT", "PEBL_MULTIPLY",
  "PEBL_NE", "PEBL_NEWLINE", "PEBL_NOT", "PEBL_OR", "PEBL_POWER",
  "PEBL_RETURN", "PEBL_RBRACE", "PEBL_RBRACKET", "PEBL_RPAREN",
  "PEBL_SUBTRACT", "PEBL_SEMI", "PEBL_STATEMENTS", "PEBL_START",
  "PEBL_VARPAIR", "PEBL_VARIABLEDATUM", "PEBL_VARLIST", "PEBL_WHILE",
  "PEBL_AND_TAIL", "PEBL_ADD_TAIL", "PEBL_ASSIGN_TAIL", "PEBL_BREAK_TAIL",
  "PEBL_COLON", "PEBL_DIVIDE_TAIL", "PEBL_EQ_TAIL", "PEBL_GE_TAIL",
  "PEBL_GT_TAIL", "PEBL_IF_TAIL", "PEBL_LE_TAIL", "PEBL_LISTITEM_TAIL",
  "PEBL_LOOP_TAIL1", "PEBL_LOOP_TAIL2", "PEBL_LT_TAIL",
  "PEBL_MULTIPLY_TAIL", "PEBL_NE_TAIL", "PEBL_NOT_TAIL", "PEBL_OR_TAIL",
  "PEBL_POWER_TAIL", "PEBL_RETURN_TAIL", "PEBL_SUBTRACT_TAIL",
  "PEBL_STATEMENTS_TAIL1", "PEBL_STATEMENTS_TAIL2", "PEBL_WHILE_TAIL",
  "PEBL_WHILE_TAIL2", "PEBL_FUNCTION_TAIL1", "PEBL_FUNCTION_TAIL2",
  "PEBL_FUNCTION_TAIL_LIBFUNCTION", "PEBL_FLOAT", "PEBL_INTEGER",
  "PEBL_STRING", "PEBL_SYMBOL", "PEBL_LOCALVAR", "PEBL_GLOBALVAR",
  "PEBL_FUNCTIONNAME", "PEBL_UMINUS", "$accept", "functions", "function",
  "functionblock", "functionsequence", "block", "sequence", "statement",
  "endstatement", "ustatement", "returnstatement", "elseifseq", "arglist",
  "list", "explist", "varlist", "variablepair", "exp", "datum", "variable",
  "nlornone", "newlines", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342
};
# endif

#define YYPACT_NINF (-101)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -19,  -101,    10,    18,   -14,  -101,   -57,   -19,   -19,  -101,
       4,  -101,  -101,   -37,   -19,  -101,  -101,   -12,    31,    -9,
      25,   -19,   -19,   247,   -19,  -101,  -101,    25,   -73,   -19,
     -19,   247,   247,  -101,  -101,  -101,    19,  -101,   467,  -101,
    -101,   103,  -101,  -101,    -5,   247,   275,    20,    -2,  -101,
     -19,   -19,   -19,   247,   247,   247,   247,   247,   -19,   247,
     -19,   -19,   -19,    26,    28,    29,   239,  -101,    32,    58,
      60,    30,   -19,  -101,  -101,   -27,   -19,   467,  -101,   -19,
     331,   351,  -101,   247,   247,   247,   247,    24,    24,    24,
      24,    24,   247,    24,   247,   247,   247,  -101,   247,   -73,
    -101,    33,   247,   247,   247,  -101,   171,  -101,  -101,  -101,
      37,   -19,    41,   -19,   275,    -1,    20,    20,   275,  -101,
      -1,   377,    77,   397,   467,   467,  -101,  -101,  -101,   -19,
    -101,   247,  -101,    44,   -19,   247,   -19,  -101,  -101,  -101,
      66,   422,    66,   -19,    12,   -19,  -101,   150,   -19,    57,
    -101,    66,   -19,    66,   247,  -101,   218,  -101,   442,   -19,
      66,    12,  -101
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      65,    67,     0,     0,    66,     1,     0,    65,    65,    68,
       0,     3,     2,     0,    65,    63,    64,     0,    36,    38,
       0,    65,    65,     0,    65,     5,     6,     0,     0,    65,
      65,     0,     0,    59,    58,    60,     0,    61,    39,    40,
      62,     0,     4,    37,     0,     0,    43,    42,    65,    57,
      65,    65,    65,     0,     0,     0,     0,     0,    65,     0,
      65,    65,    65,     0,     0,     0,     0,    13,     0,    63,
      64,     0,    65,    14,    11,     0,    65,    18,    33,    65,
      34,    65,    30,     0,     0,     0,     0,    55,    53,    52,
      54,    51,     0,    56,     0,     0,     0,    19,     0,     0,
      26,     0,     0,     0,     0,     7,     0,    16,    17,     8,
       0,    65,     0,    65,    50,    44,    45,    46,    49,    47,
      48,     0,     0,     0,    20,    21,    10,    15,    12,    65,
      32,     0,    41,     0,    65,     0,    65,     9,    35,    31,
       0,     0,     0,    65,    23,    65,    22,     0,    65,     0,
      24,     0,    65,     0,     0,    25,     0,    27,     0,    65,
       0,    28,    29
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -101,  -101,    90,    67,  -101,   -69,   -50,   -64,  -100,    34,
      -8,   -62,  -101,  -101,   -79,    73,  -101,    64,  -101,   -10,
      -7,  -101
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     7,    25,    71,    26,    72,    73,    74,    75,
      76,   150,    49,    37,    79,    17,    18,    77,    39,    40,
       3,     4
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      11,    12,   100,    19,   113,    14,   128,    20,   107,    52,
       5,    15,    16,   108,    27,    28,     1,    41,    19,     6,
      29,     9,    44,    45,   148,   149,    30,     6,    51,    10,
      21,    31,    58,     1,    52,    13,    78,    61,    32,    22,
      82,    83,   127,    84,    85,    86,    23,    15,    16,    24,
      48,    92,   138,    94,    95,    96,   128,    58,    61,    98,
      99,    97,    61,   102,   103,   106,   104,    62,   107,   109,
     105,   144,   110,   146,   112,    33,    34,    35,   130,    15,
      16,    36,   155,   132,   157,   135,   139,    38,   154,   122,
     143,   161,   127,     8,    42,    46,    47,   152,   129,   162,
     101,    43,     0,     0,   131,     0,   133,     0,    80,    81,
      63,     0,     0,     0,     0,     0,     0,    87,    88,    89,
      90,    91,   137,    93,    64,     0,     0,   140,    29,   142,
       0,     0,     0,    65,    30,     0,   147,     0,   151,    31,
       0,   153,    66,    67,     0,   156,    32,    80,   114,   115,
     116,     0,   160,    68,     0,     0,   117,    63,   118,   119,
     120,     0,   121,     0,     0,     0,   123,   124,   125,     0,
       0,    64,     0,     0,     0,    29,     0,     0,    63,     0,
      65,    30,     0,    33,    34,    35,    31,    69,    70,    36,
      67,     0,    64,    32,     0,    80,    29,     0,     0,   141,
      68,    65,    30,     0,     0,     0,     0,    31,     0,     0,
      66,   126,     0,     0,    32,     0,     0,     0,   158,     0,
       0,    68,     0,     0,     0,    63,     0,     0,     0,     0,
      33,    34,    35,     0,    69,    70,    36,     0,     0,    64,
       0,     0,     0,    29,     0,     0,    63,     0,    65,    30,
       0,    33,    34,    35,    31,    69,    70,    36,   126,     0,
      64,    32,     0,     0,    29,     0,     0,     0,    68,    65,
      30,     0,    29,     0,     0,    31,     0,     0,    30,    51,
       0,     0,    32,    31,     0,    52,     0,     0,     0,    68,
      32,    53,     0,     0,    54,    55,     0,     0,    33,    34,
      35,    56,    69,    70,    36,     0,     0,    57,    58,    59,
       0,     0,     0,    61,     0,     0,     0,     0,    62,    33,
      34,    35,     0,    69,    70,    36,     0,    33,    34,    35,
       0,    15,    16,    36,    50,    51,     0,     0,     0,   111,
       0,    52,     0,     0,     0,     0,     0,    53,     0,     0,
      54,    55,     0,     0,    50,    51,     0,    56,     0,     0,
       0,    52,     0,    57,    58,    59,     0,    53,    60,    61,
      54,    55,     0,     0,    62,     0,     0,    56,     0,     0,
      50,    51,     0,    57,    58,    59,     1,    52,    60,    61,
       0,     0,     0,    53,    62,     0,    54,    55,     0,     0,
      50,    51,     0,    56,     0,     0,     0,    52,     0,    57,
      58,    59,     0,    53,    60,    61,    54,    55,     0,   134,
      62,     0,     0,    56,     0,    50,    51,     0,     0,    57,
      58,    59,    52,     0,    60,    61,     0,     0,    53,   136,
      62,    54,    55,     0,     0,    50,    51,     0,    56,     0,
       0,     0,    52,     0,    57,    58,    59,     0,    53,    60,
      61,    54,    55,     0,   145,    62,     0,     0,    56,     0,
      50,    51,     0,     0,    57,    58,    59,    52,     0,    60,
      61,     0,     0,    53,   159,    62,    54,    55,     0,     0,
       0,     0,     0,    56,     0,     0,     0,     0,     0,    57,
      58,    59,     0,     0,    60,    61,     0,     0,     0,     0,
      62
};

static const yytype_int16 yycheck[] =
{
       7,     8,    66,    13,    83,    42,   106,    14,    35,    10,
       0,    84,    85,    40,    21,    22,    35,    24,    28,     9,
      25,    35,    29,    30,    12,    13,    31,     9,     4,    86,
      42,    36,    33,    35,    10,    31,    41,    38,    43,     8,
      42,    48,   106,    50,    51,    52,    55,    84,    85,    24,
      31,    58,   131,    60,    61,    62,   156,    33,    38,    31,
      31,    35,    38,    31,     6,    72,     6,    43,    35,    76,
      40,   140,    79,   142,    81,    80,    81,    82,    41,    84,
      85,    86,   151,    42,   153,     8,    42,    23,    31,    99,
      24,   160,   156,     3,    27,    31,    32,   147,   106,   161,
      66,    28,    -1,    -1,   111,    -1,   113,    -1,    44,    45,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,
      56,    57,   129,    59,    21,    -1,    -1,   134,    25,   136,
      -1,    -1,    -1,    30,    31,    -1,   143,    -1,   145,    36,
      -1,   148,    39,    40,    -1,   152,    43,    83,    84,    85,
      86,    -1,   159,    50,    -1,    -1,    92,     7,    94,    95,
      96,    -1,    98,    -1,    -1,    -1,   102,   103,   104,    -1,
      -1,    21,    -1,    -1,    -1,    25,    -1,    -1,     7,    -1,
      30,    31,    -1,    80,    81,    82,    36,    84,    85,    86,
      40,    -1,    21,    43,    -1,   131,    25,    -1,    -1,   135,
      50,    30,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,
      39,    40,    -1,    -1,    43,    -1,    -1,    -1,   154,    -1,
      -1,    50,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      80,    81,    82,    -1,    84,    85,    86,    -1,    -1,    21,
      -1,    -1,    -1,    25,    -1,    -1,     7,    -1,    30,    31,
      -1,    80,    81,    82,    36,    84,    85,    86,    40,    -1,
      21,    43,    -1,    -1,    25,    -1,    -1,    -1,    50,    30,
      31,    -1,    25,    -1,    -1,    36,    -1,    -1,    31,     4,
      -1,    -1,    43,    36,    -1,    10,    -1,    -1,    -1,    50,
      43,    16,    -1,    -1,    19,    20,    -1,    -1,    80,    81,
      82,    26,    84,    85,    86,    -1,    -1,    32,    33,    34,
      -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43,    80,
      81,    82,    -1,    84,    85,    86,    -1,    80,    81,    82,
      -1,    84,    85,    86,     3,     4,    -1,    -1,    -1,     8,
      -1,    10,    -1,    -1,    -1,    -1,    -1,    16,    -1,    -1,
      19,    20,    -1,    -1,     3,     4,    -1,    26,    -1,    -1,
      -1,    10,    -1,    32,    33,    34,    -1,    16,    37,    38,
      19,    20,    -1,    -1,    43,    -1,    -1,    26,    -1,    -1,
       3,     4,    -1,    32,    33,    34,    35,    10,    37,    38,
      -1,    -1,    -1,    16,    43,    -1,    19,    20,    -1,    -1,
       3,     4,    -1,    26,    -1,    -1,    -1,    10,    -1,    32,
      33,    34,    -1,    16,    37,    38,    19,    20,    -1,    42,
      43,    -1,    -1,    26,    -1,     3,     4,    -1,    -1,    32,
      33,    34,    10,    -1,    37,    38,    -1,    -1,    16,    42,
      43,    19,    20,    -1,    -1,     3,     4,    -1,    26,    -1,
      -1,    -1,    10,    -1,    32,    33,    34,    -1,    16,    37,
      38,    19,    20,    -1,    42,    43,    -1,    -1,    26,    -1,
       3,     4,    -1,    -1,    32,    33,    34,    10,    -1,    37,
      38,    -1,    -1,    16,    42,    43,    19,    20,    -1,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    32,
      33,    34,    -1,    -1,    37,    38,    -1,    -1,    -1,    -1,
      43
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    35,    89,   108,   109,     0,     9,    90,    90,    35,
      86,   108,   108,    31,    42,    84,    85,   103,   104,   107,
     108,    42,     8,    55,    24,    91,    93,   108,   108,    25,
      31,    36,    43,    80,    81,    82,    86,   101,   105,   106,
     107,   108,    91,   103,   108,   108,   105,   105,    31,   100,
       3,     4,    10,    16,    19,    20,    26,    32,    33,    34,
      37,    38,    43,     7,    21,    30,    39,    40,    50,    84,
      85,    92,    94,    95,    96,    97,    98,   105,    41,   102,
     105,   105,    42,   108,   108,   108,   108,   105,   105,   105,
     105,   105,   108,   105,   108,   108,   108,    35,    31,    31,
      95,    97,    31,     6,     6,    40,   108,    35,    40,   108,
     108,     8,   108,   102,   105,   105,   105,   105,   105,   105,
     105,   105,   107,   105,   105,   105,    40,    95,    96,    98,
      41,   108,    42,   108,    42,     8,    42,   108,   102,    42,
     108,   105,   108,    24,    93,    42,    93,   108,    12,    13,
      99,   108,    94,   108,    31,    93,   108,    93,   105,    42,
     108,    93,    99
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    88,    89,    89,    90,    90,    91,    91,    92,    92,
      93,    93,    93,    93,    94,    94,    95,    96,    97,    97,
      97,    97,    97,    97,    97,    97,    98,    99,    99,    99,
     100,   100,   101,   101,   102,   102,   103,   103,   104,   104,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   106,   106,
     106,   106,   106,   107,   107,   108,   108,   109,   109
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     3,     7,     6,     1,     4,     2,     4,
       5,     3,     5,     3,     1,     3,     2,     2,     1,     2,
       3,     3,     6,     6,     7,     8,     2,     3,     6,     7,
       2,     5,     5,     3,     1,     4,     1,     4,     1,     3,
       1,     5,     2,     2,     4,     4,     4,     4,     4,     4,
       4,     3,     3,     3,     3,     3,     3,     2,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     1,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
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
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
  yychar = YYEMPTY;
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
  case 2:
#line 161 "src/base/grammar.y"
                                           {gParseTreeHead =  new OpNode(PEBL_FUNCTIONS,(yyvsp[-1].exp),NULL, sourcefilename, yylineno);

		           (yyval.exp) = gParseTreeHead;
		}
#line 1629 "src/base/grammar.tab.cpp"
    break;

  case 3:
#line 169 "src/base/grammar.y"
                                            { gParseTreeHead = new OpNode(PEBL_FUNCTIONS, (yyvsp[-1].exp), (yyvsp[-2].exp), sourcefilename, yylineno);
		           (yyval.exp) = gParseTreeHead;
		}
#line 1637 "src/base/grammar.tab.cpp"
    break;

  case 4:
#line 177 "src/base/grammar.y"
                                                                                                       { ;
		PNode * tmpFN = new OpNode(PEBL_LAMBDAFUNCTION, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  
		tmpFN->SetFunctionName((yyvsp[-5].function));
		PNode * tmpNode = new DataNode(Variant((yyvsp[-5].function), P_DATA_FUNCTION), sourcefilename, yylineno);
		free((yyvsp[-5].function));
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, tmpFN, sourcefilename, yylineno);

        }
#line 1650 "src/base/grammar.tab.cpp"
    break;

  case 5:
#line 187 "src/base/grammar.y"
                                                                                                 { ;
		PNode * tmpFN = new OpNode(PEBL_LAMBDAFUNCTION, NULL, (yyvsp[0].exp), sourcefilename, yylineno); 
		tmpFN->SetFunctionName((yyvsp[-4].function));
		PNode * tmpNode = new DataNode(Variant((yyvsp[-4].function), P_DATA_FUNCTION), sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, tmpFN, sourcefilename, yylineno);
                free((yyvsp[-4].function));
		  }
#line 1662 "src/base/grammar.tab.cpp"
    break;

  case 6:
#line 202 "src/base/grammar.y"
                      {
		   /*When no return value is provided, return 1 (true)*/
                  DataNode* retval  = new DataNode (Variant(1), sourcefilename, yylineno);
		  OpNode *tmpReturn = new OpNode(PEBL_RETURN, retval, NULL, sourcefilename, yylineno);
    	           (yyval.exp) = new OpNode(PEBL_STATEMENTS,(yyvsp[0].exp),tmpReturn,sourcefilename,yylineno);
                 }
#line 1673 "src/base/grammar.tab.cpp"
    break;

  case 7:
#line 211 "src/base/grammar.y"
                                                                    {(yyval.exp) = (yyvsp[-1].exp);}
#line 1679 "src/base/grammar.tab.cpp"
    break;

  case 8:
#line 215 "src/base/grammar.y"
                                                       { (yyval.exp) = (yyvsp[-1].exp);}
#line 1685 "src/base/grammar.tab.cpp"
    break;

  case 9:
#line 216 "src/base/grammar.y"
                                                                { (yyval.exp) = new OpNode(PEBL_STATEMENTS, (yyvsp[-3].exp), (yyvsp[-1].exp), sourcefilename, yylineno);}
#line 1691 "src/base/grammar.tab.cpp"
    break;

  case 10:
#line 220 "src/base/grammar.y"
                                                                     { (yyval.exp) = (yyvsp[-2].exp);}
#line 1697 "src/base/grammar.tab.cpp"
    break;

  case 11:
#line 221 "src/base/grammar.y"
                                              {(yyval.exp) = (yyvsp[0].exp);}
#line 1703 "src/base/grammar.tab.cpp"
    break;

  case 12:
#line 222 "src/base/grammar.y"
                                                                 {
  (yyval.exp)  = new OpNode(PEBL_STATEMENTS, (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1710 "src/base/grammar.tab.cpp"
    break;

  case 13:
#line 224 "src/base/grammar.y"
                                         { (yyval.exp) = new DataNode (Variant(0), sourcefilename, yylineno);}
#line 1716 "src/base/grammar.tab.cpp"
    break;

  case 14:
#line 230 "src/base/grammar.y"
                                       { (yyval.exp) = (yyvsp[0].exp); }
#line 1722 "src/base/grammar.tab.cpp"
    break;

  case 15:
#line 233 "src/base/grammar.y"
                                                   { (yyval.exp) = new OpNode(PEBL_STATEMENTS, (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1728 "src/base/grammar.tab.cpp"
    break;

  case 16:
#line 237 "src/base/grammar.y"
                                   {(yyval.exp) = (yyvsp[-1].exp);}
#line 1734 "src/base/grammar.tab.cpp"
    break;

  case 17:
#line 241 "src/base/grammar.y"
                                     {(yyval.exp) = (yyvsp[-1].exp);}
#line 1740 "src/base/grammar.tab.cpp"
    break;

  case 18:
#line 250 "src/base/grammar.y"
                                      {(yyval.exp) = (yyvsp[0].exp);}
#line 1746 "src/base/grammar.tab.cpp"
    break;

  case 19:
#line 252 "src/base/grammar.y"
                                          {(yyval.exp) = new OpNode(PEBL_BREAK, NULL, NULL, sourcefilename, yylineno);}
#line 1752 "src/base/grammar.tab.cpp"
    break;

  case 20:
#line 255 "src/base/grammar.y"
                { 
	         Variant tmpV((yyvsp[-2].symbol),P_DATA_LOCALVARIABLE);       /*create a new temporary variant*/
		 /*free($1);*/
		 PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);        /*create basic pnode*/
		 (yyval.exp) = new OpNode(PEBL_ASSIGN, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);   /*Use symbol node in assignment node*/
		}
#line 1763 "src/base/grammar.tab.cpp"
    break;

  case 21:
#line 264 "src/base/grammar.y"
                { 
	        Variant tmpV((yyvsp[-2].symbol),P_DATA_GLOBALVARIABLE);      /*create a new temporary variant*/
		PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);        /*create basic pnode*/
		(yyval.exp) = new OpNode(PEBL_ASSIGN, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);   /*Use symbol node in assignment node*/
		}
#line 1773 "src/base/grammar.tab.cpp"
    break;

  case 22:
#line 271 "src/base/grammar.y"
                                                                       {;
		(yyval.exp) = new OpNode(PEBL_WHILE, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1780 "src/base/grammar.tab.cpp"
    break;

  case 23:
#line 284 "src/base/grammar.y"
                                                                    {
		(yyval.exp) = new OpNode(PEBL_IF, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1787 "src/base/grammar.tab.cpp"
    break;

  case 24:
#line 290 "src/base/grammar.y"
                                                                             {
		/*First make the else node*/
		PNode * tmpNode = new OpNode(PEBL_ELSE, (yyvsp[-1].exp), (yyvsp[0].exp), sourcefilename, yylineno);
		/*Put the else node in the IF node*/
		(yyval.exp) = new OpNode(PEBL_IFELSE, (yyvsp[-4].exp), tmpNode, sourcefilename, yylineno); }
#line 1797 "src/base/grammar.tab.cpp"
    break;

  case 25:
#line 306 "src/base/grammar.y"
                                                                                          {
		PNode * tmpNode = new OpNode(PEBL_VARIABLEDATUM, (yyvsp[-5].exp), (yyvsp[-3].exp), sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_LOOP, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1805 "src/base/grammar.tab.cpp"
    break;

  case 26:
#line 313 "src/base/grammar.y"
                                          {(yyval.exp) = new OpNode(PEBL_RETURN, (yyvsp[0].exp), NULL, sourcefilename, yylineno);}
#line 1811 "src/base/grammar.tab.cpp"
    break;

  case 27:
#line 318 "src/base/grammar.y"
                                      {

		(yyval.exp) = (yyvsp[0].exp); }
#line 1819 "src/base/grammar.tab.cpp"
    break;

  case 28:
#line 325 "src/base/grammar.y"
                                                                {
		/*First make the else node*/

		  (yyval.exp) =  new OpNode(PEBL_IF, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);

   }
#line 1830 "src/base/grammar.tab.cpp"
    break;

  case 29:
#line 331 "src/base/grammar.y"
                                                                         {

		/*First make the else node*/
		PNode * tmpNode = new OpNode(PEBL_ELSE, (yyvsp[-1].exp), (yyvsp[0].exp), sourcefilename, yylineno);
		/*Put the else node in the IF node*/
		(yyval.exp) = new OpNode(PEBL_IFELSE, (yyvsp[-4].exp), tmpNode, sourcefilename, yylineno); }
#line 1841 "src/base/grammar.tab.cpp"
    break;

  case 30:
#line 348 "src/base/grammar.y"
                                                            {(yyval.exp) = new OpNode(PEBL_ARGLIST, NULL, NULL, sourcefilename, yylineno);}
#line 1847 "src/base/grammar.tab.cpp"
    break;

  case 31:
#line 352 "src/base/grammar.y"
                                                                     {(yyval.exp) = new OpNode(PEBL_ARGLIST, (yyvsp[-2].exp), NULL, sourcefilename, yylineno);}
#line 1853 "src/base/grammar.tab.cpp"
    break;

  case 32:
#line 362 "src/base/grammar.y"
                                                                      {(yyval.exp) = new OpNode(PEBL_LISTHEAD,(yyvsp[-2].exp), NULL, sourcefilename, yylineno);}
#line 1859 "src/base/grammar.tab.cpp"
    break;

  case 33:
#line 365 "src/base/grammar.y"
                                                               {(yyval.exp) = new OpNode(PEBL_LISTHEAD, NULL, NULL, sourcefilename, yylineno);}
#line 1865 "src/base/grammar.tab.cpp"
    break;

  case 34:
#line 369 "src/base/grammar.y"
                                                      {(yyval.exp) = new OpNode(PEBL_LISTITEM, (yyvsp[0].exp), NULL, sourcefilename, yylineno);}
#line 1871 "src/base/grammar.tab.cpp"
    break;

  case 35:
#line 373 "src/base/grammar.y"
                                                      {(yyval.exp) = new OpNode(PEBL_LISTITEM, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1877 "src/base/grammar.tab.cpp"
    break;

  case 36:
#line 381 "src/base/grammar.y"
                                  {(yyval.exp) = new OpNode(PEBL_VARLIST, (yyvsp[0].exp), NULL, sourcefilename, yylineno);}
#line 1883 "src/base/grammar.tab.cpp"
    break;

  case 37:
#line 385 "src/base/grammar.y"
                                                           {(yyval.exp) = new OpNode(PEBL_VARLIST,(yyvsp[-3].exp),(yyvsp[0].exp), sourcefilename, yylineno);}
#line 1889 "src/base/grammar.tab.cpp"
    break;

  case 38:
#line 392 "src/base/grammar.y"
                         {(yyval.exp) = (yyvsp[0].exp);}
#line 1895 "src/base/grammar.tab.cpp"
    break;

  case 39:
#line 394 "src/base/grammar.y"
                                       {
		       (yyval.exp)  = new OpNode(PEBL_VARPAIR,(yyvsp[-2].exp), (yyvsp[0].exp),sourcefilename,yylineno);
	       }
#line 1903 "src/base/grammar.tab.cpp"
    break;

  case 40:
#line 402 "src/base/grammar.y"
                                        { (yyval.exp) = (yyvsp[0].exp);}
#line 1909 "src/base/grammar.tab.cpp"
    break;

  case 41:
#line 405 "src/base/grammar.y"
                                                                 {(yyval.exp) = (yyvsp[-2].exp);}
#line 1915 "src/base/grammar.tab.cpp"
    break;

  case 42:
#line 408 "src/base/grammar.y"
                                                        {
		Variant tmpV = 0;
		PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_SUBTRACT, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1924 "src/base/grammar.tab.cpp"
    break;

  case 43:
#line 414 "src/base/grammar.y"
                                                    {(yyval.exp) = new OpNode(PEBL_NOT, (yyvsp[0].exp), NULL, sourcefilename, yylineno); }
#line 1930 "src/base/grammar.tab.cpp"
    break;

  case 44:
#line 417 "src/base/grammar.y"
                                                  { (yyval.exp) = new OpNode(PEBL_ADD, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  }
#line 1936 "src/base/grammar.tab.cpp"
    break;

  case 45:
#line 421 "src/base/grammar.y"
                                                   { (yyval.exp) = new OpNode(PEBL_DIVIDE,(yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  }
#line 1942 "src/base/grammar.tab.cpp"
    break;

  case 46:
#line 424 "src/base/grammar.y"
                                                  { (yyval.exp) = new OpNode(PEBL_MULTIPLY, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1948 "src/base/grammar.tab.cpp"
    break;

  case 47:
#line 427 "src/base/grammar.y"
                                               { (yyval.exp) = new OpNode(PEBL_POWER, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1954 "src/base/grammar.tab.cpp"
    break;

  case 48:
#line 430 "src/base/grammar.y"
                                                  { (yyval.exp) = new OpNode(PEBL_SUBTRACT, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1960 "src/base/grammar.tab.cpp"
    break;

  case 49:
#line 433 "src/base/grammar.y"
                                            { (yyval.exp) = new OpNode(PEBL_OR, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1966 "src/base/grammar.tab.cpp"
    break;

  case 50:
#line 436 "src/base/grammar.y"
                                            { (yyval.exp) = new OpNode(PEBL_AND, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1972 "src/base/grammar.tab.cpp"
    break;

  case 51:
#line 440 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_LT,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1978 "src/base/grammar.tab.cpp"
    break;

  case 52:
#line 443 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_GT,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1984 "src/base/grammar.tab.cpp"
    break;

  case 53:
#line 446 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_GE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1990 "src/base/grammar.tab.cpp"
    break;

  case 54:
#line 449 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_LE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1996 "src/base/grammar.tab.cpp"
    break;

  case 55:
#line 452 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_EQ,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 2002 "src/base/grammar.tab.cpp"
    break;

  case 56:
#line 455 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_NE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 2008 "src/base/grammar.tab.cpp"
    break;

  case 57:
#line 464 "src/base/grammar.y"
                                          {


		  /*Memory leak here, by electricfence*/
		Variant v = Variant((yyvsp[-1].function),P_DATA_FUNCTION);
		PNode * tmpNode = new DataNode(v, sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);
		free((yyvsp[-1].function));
		}
#line 2022 "src/base/grammar.tab.cpp"
    break;

  case 58:
#line 478 "src/base/grammar.y"
                                         { (yyval.exp) = new DataNode ((yyvsp[0].iValue), sourcefilename, yylineno); }
#line 2028 "src/base/grammar.tab.cpp"
    break;

  case 59:
#line 481 "src/base/grammar.y"
                                         { (yyval.exp) = new DataNode ((yyvsp[0].fValue), sourcefilename, yylineno);}
#line 2034 "src/base/grammar.tab.cpp"
    break;

  case 60:
#line 484 "src/base/grammar.y"
                                         {
	  Variant tmpV((yyvsp[0].strValue));            /*create a new temporary variant*/
	  free((yyvsp[0].strValue));
	  (yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);

                        }
#line 2045 "src/base/grammar.tab.cpp"
    break;

  case 61:
#line 491 "src/base/grammar.y"
                                         { (yyval.exp) = (yyvsp[0].exp);}
#line 2051 "src/base/grammar.tab.cpp"
    break;

  case 62:
#line 494 "src/base/grammar.y"
                                         { (yyval.exp) = (yyvsp[0].exp);}
#line 2057 "src/base/grammar.tab.cpp"
    break;

  case 63:
#line 499 "src/base/grammar.y"
                                           { 
		Variant tmpV((yyvsp[0].symbol), P_DATA_LOCALVARIABLE);           /*create a new temporary variant*/;
		(yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);                        /*Make a new variable node here.*/
                free((yyvsp[0].symbol));
                }
#line 2067 "src/base/grammar.tab.cpp"
    break;

  case 64:
#line 504 "src/base/grammar.y"
                                            { 
		Variant tmpV((yyvsp[0].symbol), P_DATA_GLOBALVARIABLE);          /*create a new temporary variant*/;
		(yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);  /*Make a new variable node here.*/
		free((yyvsp[0].symbol));
		 }
#line 2077 "src/base/grammar.tab.cpp"
    break;

  case 65:
#line 513 "src/base/grammar.y"
                       {/*nothing*/;}
#line 2083 "src/base/grammar.tab.cpp"
    break;

  case 66:
#line 514 "src/base/grammar.y"
                                 {/**/;}
#line 2089 "src/base/grammar.tab.cpp"
    break;

  case 67:
#line 518 "src/base/grammar.y"
                                      {/**/;}
#line 2095 "src/base/grammar.tab.cpp"
    break;

  case 68:
#line 519 "src/base/grammar.y"
                                      {/**/;}
#line 2101 "src/base/grammar.tab.cpp"
    break;


#line 2105 "src/base/grammar.tab.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 525 "src/base/grammar.y"


void yyerror (char *error)
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
