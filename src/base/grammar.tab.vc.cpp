/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
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
     PEBL_VARIABLEDATUM = 302,
     PEBL_VARLIST = 303,
     PEBL_WHILE = 304,
     PEBL_FLOAT = 305,
     PEBL_INTEGER = 306,
     PEBL_STRING = 307,
     PEBL_SYMBOL = 308,
     PEBL_LOCALVAR = 309,
     PEBL_GLOBALVAR = 310,
     PEBL_FUNCTIONNAME = 311,
     PEBL_UMINUS = 312
   };
#endif
#define PEBL_AND 258
#define PEBL_ADD 259
#define PEBL_ARGLIST 260
#define PEBL_ASSIGN 261
#define PEBL_BREAK 262
#define PEBL_COMMA 263
#define PEBL_DEFINE 264
#define PEBL_DIVIDE 265
#define PEBL_DOT 266
#define PEBL_ELSE 267
#define PEBL_ELSEIF 268
#define PEBL_END 269
#define PEBL_EOF 270
#define PEBL_EQ 271
#define PEBL_FUNCTION 272
#define PEBL_FUNCTIONS 273
#define PEBL_GE 274
#define PEBL_GT 275
#define PEBL_IF 276
#define PEBL_IFELSE 277
#define PEBL_LAMBDAFUNCTION 278
#define PEBL_LBRACE 279
#define PEBL_LBRACKET 280
#define PEBL_LE 281
#define PEBL_LIBRARYFUNCTION 282
#define PEBL_LISTHEAD 283
#define PEBL_LISTITEM 284
#define PEBL_LOOP 285
#define PEBL_LPAREN 286
#define PEBL_LT 287
#define PEBL_MULTIPLY 288
#define PEBL_NE 289
#define PEBL_NEWLINE 290
#define PEBL_NOT 291
#define PEBL_OR 292
#define PEBL_POWER 293
#define PEBL_RETURN 294
#define PEBL_RBRACE 295
#define PEBL_RBRACKET 296
#define PEBL_RPAREN 297
#define PEBL_SUBTRACT 298
#define PEBL_SEMI 299
#define PEBL_STATEMENTS 300
#define PEBL_START 301
#define PEBL_VARIABLEDATUM 302
#define PEBL_VARLIST 303
#define PEBL_WHILE 304
#define PEBL_FLOAT 305
#define PEBL_INTEGER 306
#define PEBL_STRING 307
#define PEBL_SYMBOL 308
#define PEBL_LOCALVAR 309
#define PEBL_GLOBALVAR 310
#define PEBL_FUNCTIONNAME 311
#define PEBL_UMINUS 312




/* Copy the first part of user declarations.  */
#line 1 "src/base/grammar.y"

  
#include "PNode.h"


#include <stdio.h>
//#include <iostream.h>
#include <cctype>
#include <cstring>
#include <vector>
#include <stack>

  
  // Prototypes to keep the compiler happy
  void yyerror (char *error);
  int  yylex ();
  extern FILE * yyin;
  PNode * gParseTreeHead;
  PNode * parse(const char* filename);
  extern int yylineno;
  char* sourcefilename;




/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 30 "src/base/grammar.y"
typedef union YYSTYPE {
  
  long int    iValue;  /* For the lexical analyser. NUMBER tokens */
  long double fValue;
  char        *strValue; 
  PNode       *exp;    /* For expressions. */
  char        *symbol; /* The name of a variable*/
  char        *function; 
} YYSTYPE;
/* Line 190 of yacc.c.  */
#line 225 "src/base/grammar.tab.cpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 237 "src/base/grammar.tab.cpp"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   450

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  58
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  21
/* YYNRULES -- Number of rules. */
#define YYNRULES  65
/* YYNRULES -- Number of states. */
#define YYNSTATES  157

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   312

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
      55,    56,    57
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     7,    11,    19,    26,    28,    33,    36,
      41,    47,    51,    57,    59,    63,    66,    69,    71,    74,
      78,    82,    89,    96,   104,   113,   116,   120,   127,   135,
     138,   143,   148,   152,   154,   159,   161,   166,   168,   174,
     177,   180,   185,   190,   195,   200,   205,   210,   215,   219,
     223,   227,   231,   235,   239,   242,   244,   246,   248,   250,
     252,   254,   256,   257,   259,   261
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      59,     0,    -1,    77,    60,    77,    -1,    59,    60,    77,
      -1,     9,    56,    31,    73,    42,    77,    61,    -1,     9,
      56,    31,    42,    77,    61,    -1,    63,    -1,    24,    77,
      62,    40,    -1,    68,    77,    -1,    64,    77,    68,    77,
      -1,    24,    77,    64,    77,    40,    -1,    24,    77,    66,
      -1,    24,    77,    64,    77,    66,    -1,    65,    -1,    64,
      77,    65,    -1,    67,    35,    -1,    67,    40,    -1,    74,
      -1,     7,    35,    -1,    54,     6,    74,    -1,    55,     6,
      74,    -1,    49,    31,    74,    42,    77,    63,    -1,    21,
      31,    74,    42,    77,    63,    -1,    21,    31,    74,    42,
      77,    63,    69,    -1,    30,    31,    76,     8,    74,    42,
      77,    63,    -1,    39,    65,    -1,    12,    77,    63,    -1,
      13,    31,    74,    42,    77,    63,    -1,    13,    31,    74,
      42,    77,    63,    69,    -1,    31,    42,    -1,    31,    77,
      72,    42,    -1,    25,    77,    72,    41,    -1,    25,    77,
      41,    -1,    74,    -1,    74,     8,    77,    72,    -1,    76,
      -1,    76,     8,    77,    73,    -1,    75,    -1,    31,    77,
      74,    77,    42,    -1,    43,    74,    -1,    36,    74,    -1,
      74,     4,    77,    74,    -1,    74,    10,    77,    74,    -1,
      74,    33,    77,    74,    -1,    74,    38,    77,    74,    -1,
      74,    43,    77,    74,    -1,    74,    37,    77,    74,    -1,
      74,     3,    77,    74,    -1,    74,    32,    74,    -1,    74,
      20,    74,    -1,    74,    19,    74,    -1,    74,    26,    74,
      -1,    74,    16,    74,    -1,    74,    34,    74,    -1,    56,
      70,    -1,    51,    -1,    50,    -1,    52,    -1,    71,    -1,
      76,    -1,    54,    -1,    55,    -1,    -1,    78,    -1,    35,
      -1,    78,    35,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   119,   119,   127,   135,   144,   156,   157,   161,   162,
     166,   167,   168,   176,   179,   183,   187,   196,   198,   200,
     209,   217,   230,   236,   252,   259,   263,   270,   276,   294,
     298,   308,   311,   315,   319,   327,   331,   337,   340,   343,
     349,   352,   356,   359,   362,   365,   368,   371,   375,   378,
     381,   384,   387,   390,   399,   409,   412,   415,   422,   425,
     430,   435,   442,   443,   447,   448
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
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
  "PEBL_VARIABLEDATUM", "PEBL_VARLIST", "PEBL_WHILE", "PEBL_FLOAT",
  "PEBL_INTEGER", "PEBL_STRING", "PEBL_SYMBOL", "PEBL_LOCALVAR",
  "PEBL_GLOBALVAR", "PEBL_FUNCTIONNAME", "PEBL_UMINUS", "$accept",
  "functions", "function", "functionblock", "functionsequence", "block",
  "sequence", "statement", "endstatement", "ustatement", "returnstatement",
  "elseifseq", "arglist", "list", "explist", "varlist", "exp", "datum",
  "variable", "nlornone", "newlines", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    58,    59,    59,    60,    60,    61,    61,    62,    62,
      63,    63,    63,    64,    64,    65,    66,    67,    67,    67,
      67,    67,    67,    67,    67,    68,    69,    69,    69,    70,
      70,    71,    71,    72,    72,    73,    73,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    75,    75,    75,    75,    75,
      76,    76,    77,    77,    78,    78
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     3,     3,     7,     6,     1,     4,     2,     4,
       5,     3,     5,     1,     3,     2,     2,     1,     2,     3,
       3,     6,     6,     7,     8,     2,     3,     6,     7,     2,
       4,     4,     3,     1,     4,     1,     4,     1,     5,     2,
       2,     4,     4,     4,     4,     4,     4,     4,     3,     3,
       3,     3,     3,     3,     2,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
      62,    64,     0,     0,    63,     1,     0,    62,    62,    65,
       0,     3,     2,     0,    62,    60,    61,     0,    35,     0,
      62,    62,    62,     5,     6,     0,     0,     0,     4,    36,
       0,     0,    62,     0,    62,     0,     0,     0,     0,    56,
      55,    57,    60,    61,     0,     0,    62,    13,    11,     0,
      62,    58,    17,    37,    59,    18,     0,     0,     0,     0,
      40,    25,     0,    39,     0,     0,     0,    62,    54,     7,
       0,    15,    16,     8,    62,    62,    62,     0,     0,     0,
       0,     0,    62,     0,    62,    62,    62,     0,    32,     0,
      33,     0,    62,     0,    19,    20,    29,     0,    10,    14,
      12,    62,     0,     0,     0,    52,    50,    49,    51,    48,
       0,    53,     0,     0,     0,    62,    31,    62,     0,     0,
      62,     0,     9,    47,    41,    42,    43,    46,    44,    45,
       0,     0,     0,    38,     0,    30,    62,    22,    34,    62,
      21,     0,    62,     0,    23,     0,    62,     0,     0,    24,
       0,    26,     0,    62,     0,    27,    28
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     2,     7,    23,    45,    24,    46,    47,    48,    49,
      50,   144,    68,    51,    89,    17,    52,    53,    54,     3,
       4
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -90
static const short int yypact[] =
{
     -19,   -90,    11,     9,   -11,   -90,   -12,   -19,   -19,   -90,
      15,   -90,   -90,   -33,   -19,   -90,   -90,    16,    49,    27,
     -19,   -19,   -19,   -90,   -90,    27,   -22,   126,   -90,   -90,
      29,    34,   -19,    35,   -19,   394,   180,   394,    39,   -90,
     -90,   -90,    65,    66,    42,    36,   -19,   -90,   -90,    -9,
     -19,   -90,   358,   -90,   -90,   -90,   394,   377,   -22,   394,
     378,   -90,    47,   -90,   394,   394,   394,   -32,   -90,   -90,
      -2,   -90,   -90,   -90,   -19,   -19,   -19,   394,   394,   394,
     394,   394,   -19,   394,   -19,   -19,   -19,   223,   -90,    33,
     243,    79,   268,   288,   358,   358,   -90,   394,   -90,   -90,
     -90,   -19,   394,   394,   394,     2,     2,     2,     2,     2,
     394,     2,   394,   394,   394,   -19,   -90,   -19,   394,    50,
     -19,    51,   -90,   378,    53,    57,    57,   378,   -90,    53,
      72,   394,   313,   -90,    72,   -90,   -19,    43,   -90,   -19,
     -90,   180,   -19,    67,   -90,    72,   -19,    72,   394,   -90,
     143,   -90,   333,   -19,    72,    43,   -90
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -90,   -90,    94,    74,   -90,    55,   -41,   -34,   -66,    70,
      41,   -46,   -90,   -90,   -89,    86,    24,   -90,     4,    -7,
     -90
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      11,    12,    61,     1,   100,    30,    75,    19,   121,    14,
      96,     5,    76,    25,    26,    27,     1,    18,     6,    31,
       6,    15,    16,    32,     9,    57,    71,    59,    33,    34,
      18,    72,    15,    16,    35,    82,    99,    36,    98,    70,
      85,    37,   138,    73,    10,    86,    13,    38,    39,    40,
      41,    22,    42,    43,    44,   142,   143,    21,    20,    60,
      97,    63,    91,    76,    55,    56,    58,   102,   103,   104,
      64,    65,    66,    67,   116,   110,    69,   112,   113,   114,
      87,    90,    71,    92,   100,   119,    82,   118,    93,    94,
      95,    85,   133,   135,   122,    85,   136,     8,   148,    28,
     146,   105,   106,   107,   108,   109,    62,   111,   130,   156,
     131,   101,    29,   134,     0,     0,    99,     0,     0,     0,
       0,    90,     0,     0,     0,     0,   123,   124,   125,   141,
       0,     0,   145,    30,   126,   147,   127,   128,   129,   150,
       0,     0,   132,     0,     0,     0,   154,    31,     0,     0,
      30,    32,     0,     0,     0,    90,    33,    34,     0,     0,
       0,     0,    35,     0,    31,    36,     0,     0,    32,    37,
       0,     0,   152,    33,    34,    38,    39,    40,    41,    35,
      42,    43,    44,    98,     0,   137,    37,    30,     0,   140,
       0,     0,    38,    39,    40,    41,     0,    42,    43,    44,
     149,    31,   151,     0,     0,    32,     0,     0,     0,   155,
      33,    34,     0,     0,     0,     0,    35,     0,     0,     0,
       0,     0,     0,    37,     0,     0,    74,    75,     0,    38,
      39,    40,    41,    76,    42,    43,    44,     0,     0,    77,
       0,     0,    78,    79,     0,     0,    74,    75,     0,    80,
       0,   117,     0,    76,     0,    81,    82,    83,     0,    77,
      84,    85,    78,    79,     0,   115,    86,     0,     0,    80,
       0,    74,    75,     0,     0,    81,    82,    83,    76,     0,
      84,    85,     0,     0,    77,     0,    86,    78,    79,     0,
       0,    74,    75,     0,    80,     0,     0,     0,    76,     0,
      81,    82,    83,     1,    77,    84,    85,    78,    79,     0,
       0,    86,     0,     0,    80,     0,    74,    75,     0,     0,
      81,    82,    83,    76,     0,    84,    85,     0,     0,    77,
     120,    86,    78,    79,     0,     0,    74,    75,     0,    80,
       0,     0,     0,    76,     0,    81,    82,    83,     0,    77,
      84,    85,    78,    79,     0,   139,    86,     0,     0,    80,
       0,    74,    75,     0,     0,    81,    82,    83,    76,     0,
      84,    85,     0,     0,    77,   153,    86,    78,    79,     0,
       0,     0,    75,     0,    80,     0,     0,     0,    76,     0,
      81,    82,    83,     0,    77,    84,    85,    78,    79,     0,
       0,    86,    32,     0,    80,     0,     0,     0,    34,     0,
      81,    82,    83,    35,     0,     0,    85,     0,    88,    32,
      37,    86,     0,     0,     0,    34,     0,    39,    40,    41,
      35,    15,    16,    44,     0,     0,     0,    37,     0,     0,
       0,     0,     0,     0,    39,    40,    41,     0,    15,    16,
      44
};

static const short int yycheck[] =
{
       7,     8,    36,    35,    70,     7,     4,    14,    97,    42,
      42,     0,    10,    20,    21,    22,    35,    13,     9,    21,
       9,    54,    55,    25,    35,    32,    35,    34,    30,    31,
      26,    40,    54,    55,    36,    33,    70,    39,    40,    46,
      38,    43,   131,    50,    56,    43,    31,    49,    50,    51,
      52,    24,    54,    55,    56,    12,    13,     8,    42,    35,
      67,    37,    58,    10,    35,    31,    31,    74,    75,    76,
      31,     6,     6,    31,    41,    82,    40,    84,    85,    86,
      56,    57,    35,    59,   150,    92,    33,     8,    64,    65,
      66,    38,    42,    42,   101,    38,    24,     3,    31,    25,
     141,    77,    78,    79,    80,    81,    36,    83,   115,   155,
     117,    70,    26,   120,    -1,    -1,   150,    -1,    -1,    -1,
      -1,    97,    -1,    -1,    -1,    -1,   102,   103,   104,   136,
      -1,    -1,   139,     7,   110,   142,   112,   113,   114,   146,
      -1,    -1,   118,    -1,    -1,    -1,   153,    21,    -1,    -1,
       7,    25,    -1,    -1,    -1,   131,    30,    31,    -1,    -1,
      -1,    -1,    36,    -1,    21,    39,    -1,    -1,    25,    43,
      -1,    -1,   148,    30,    31,    49,    50,    51,    52,    36,
      54,    55,    56,    40,    -1,   130,    43,     7,    -1,   134,
      -1,    -1,    49,    50,    51,    52,    -1,    54,    55,    56,
     145,    21,   147,    -1,    -1,    25,    -1,    -1,    -1,   154,
      30,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    -1,    -1,     3,     4,    -1,    49,
      50,    51,    52,    10,    54,    55,    56,    -1,    -1,    16,
      -1,    -1,    19,    20,    -1,    -1,     3,     4,    -1,    26,
      -1,     8,    -1,    10,    -1,    32,    33,    34,    -1,    16,
      37,    38,    19,    20,    -1,    42,    43,    -1,    -1,    26,
      -1,     3,     4,    -1,    -1,    32,    33,    34,    10,    -1,
      37,    38,    -1,    -1,    16,    -1,    43,    19,    20,    -1,
      -1,     3,     4,    -1,    26,    -1,    -1,    -1,    10,    -1,
      32,    33,    34,    35,    16,    37,    38,    19,    20,    -1,
      -1,    43,    -1,    -1,    26,    -1,     3,     4,    -1,    -1,
      32,    33,    34,    10,    -1,    37,    38,    -1,    -1,    16,
      42,    43,    19,    20,    -1,    -1,     3,     4,    -1,    26,
      -1,    -1,    -1,    10,    -1,    32,    33,    34,    -1,    16,
      37,    38,    19,    20,    -1,    42,    43,    -1,    -1,    26,
      -1,     3,     4,    -1,    -1,    32,    33,    34,    10,    -1,
      37,    38,    -1,    -1,    16,    42,    43,    19,    20,    -1,
      -1,    -1,     4,    -1,    26,    -1,    -1,    -1,    10,    -1,
      32,    33,    34,    -1,    16,    37,    38,    19,    20,    -1,
      -1,    43,    25,    -1,    26,    -1,    -1,    -1,    31,    -1,
      32,    33,    34,    36,    -1,    -1,    38,    -1,    41,    25,
      43,    43,    -1,    -1,    -1,    31,    -1,    50,    51,    52,
      36,    54,    55,    56,    -1,    -1,    -1,    43,    -1,    -1,
      -1,    -1,    -1,    -1,    50,    51,    52,    -1,    54,    55,
      56
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    35,    59,    77,    78,     0,     9,    60,    60,    35,
      56,    77,    77,    31,    42,    54,    55,    73,    76,    77,
      42,     8,    24,    61,    63,    77,    77,    77,    61,    73,
       7,    21,    25,    30,    31,    36,    39,    43,    49,    50,
      51,    52,    54,    55,    56,    62,    64,    65,    66,    67,
      68,    71,    74,    75,    76,    35,    31,    77,    31,    77,
      74,    65,    67,    74,    31,     6,     6,    31,    70,    40,
      77,    35,    40,    77,     3,     4,    10,    16,    19,    20,
      26,    32,    33,    34,    37,    38,    43,    74,    41,    72,
      74,    76,    74,    74,    74,    74,    42,    77,    40,    65,
      66,    68,    77,    77,    77,    74,    74,    74,    74,    74,
      77,    74,    77,    77,    77,    42,    41,     8,     8,    77,
      42,    72,    77,    74,    74,    74,    74,    74,    74,    74,
      77,    77,    74,    42,    77,    42,    24,    63,    72,    42,
      63,    77,    12,    13,    69,    77,    64,    77,    31,    63,
      77,    63,    74,    42,    77,    63,    69
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
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
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  yyvsp[0] = yylval;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

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
#line 119 "src/base/grammar.y"
    {gParseTreeHead =  new OpNode(PEBL_FUNCTIONS,(yyvsp[-1].exp),NULL, sourcefilename, yylineno);

		           (yyval.exp) = gParseTreeHead;
		;}
    break;

  case 3:
#line 127 "src/base/grammar.y"
    { gParseTreeHead = new OpNode(PEBL_FUNCTIONS, (yyvsp[-1].exp), (yyvsp[-2].exp), sourcefilename, yylineno);
		           (yyval.exp) = gParseTreeHead;
		;}
    break;

  case 4:
#line 135 "src/base/grammar.y"
    { ;
		PNode * tmpFN = new OpNode(PEBL_LAMBDAFUNCTION, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  
		PNode * tmpNode = new DataNode(Variant((yyvsp[-5].function), P_DATA_FUNCTION), sourcefilename, yylineno);
		free((yyvsp[-5].function));
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, tmpFN, sourcefilename, yylineno);

        ;}
    break;

  case 5:
#line 144 "src/base/grammar.y"
    { ;
		PNode * tmpFN = new OpNode(PEBL_LAMBDAFUNCTION, NULL, (yyvsp[0].exp), sourcefilename, yylineno);  
		PNode * tmpNode = new DataNode(Variant((yyvsp[-4].function), P_DATA_FUNCTION), sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, tmpFN, sourcefilename, yylineno);
        free((yyvsp[-4].function));
		  ;}
    break;

  case 6:
#line 156 "src/base/grammar.y"
    {(yyval.exp) = (yyvsp[0].exp);;}
    break;

  case 7:
#line 157 "src/base/grammar.y"
    {(yyval.exp) = (yyvsp[-1].exp);;}
    break;

  case 8:
#line 161 "src/base/grammar.y"
    { (yyval.exp) = (yyvsp[-1].exp);;}
    break;

  case 9:
#line 162 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_STATEMENTS, (yyvsp[-3].exp), (yyvsp[-1].exp), sourcefilename, yylineno);;}
    break;

  case 10:
#line 166 "src/base/grammar.y"
    { (yyval.exp) = (yyvsp[-2].exp);;}
    break;

  case 11:
#line 167 "src/base/grammar.y"
    {(yyval.exp) = (yyvsp[0].exp);;}
    break;

  case 12:
#line 168 "src/base/grammar.y"
    {
                     (yyval.exp)  = new OpNode(PEBL_STATEMENTS, (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);
;}
    break;

  case 13:
#line 176 "src/base/grammar.y"
    { (yyval.exp) = (yyvsp[0].exp); ;}
    break;

  case 14:
#line 179 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_STATEMENTS, (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 15:
#line 183 "src/base/grammar.y"
    {(yyval.exp) = (yyvsp[-1].exp);;}
    break;

  case 16:
#line 187 "src/base/grammar.y"
    {(yyval.exp) = (yyvsp[-1].exp);;}
    break;

  case 17:
#line 196 "src/base/grammar.y"
    {(yyval.exp) = (yyvsp[0].exp);;}
    break;

  case 18:
#line 198 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_BREAK, NULL, NULL, sourcefilename, yylineno);;}
    break;

  case 19:
#line 201 "src/base/grammar.y"
    { 
	        Variant tmpV((yyvsp[-2].symbol),P_DATA_LOCALVARIABLE);       /*create a new temporary variant*/
		    free((yyvsp[-2].symbol));
			PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);        /*create basic pnode*/
		    (yyval.exp) = new OpNode(PEBL_ASSIGN, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);   /*Use symbol node in assignment node*/
		;}
    break;

  case 20:
#line 210 "src/base/grammar.y"
    { 
	        Variant tmpV((yyvsp[-2].symbol),P_DATA_GLOBALVARIABLE);      /*create a new temporary variant*/
		PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);        /*create basic pnode*/
		(yyval.exp) = new OpNode(PEBL_ASSIGN, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);   /*Use symbol node in assignment node*/
		;}
    break;

  case 21:
#line 217 "src/base/grammar.y"
    {;
		(yyval.exp) = new OpNode(PEBL_WHILE, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); ;}
    break;

  case 22:
#line 230 "src/base/grammar.y"
    {
		(yyval.exp) = new OpNode(PEBL_IF, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); ;}
    break;

  case 23:
#line 236 "src/base/grammar.y"
    {
		/*First make the else node*/
		PNode * tmpNode = new OpNode(PEBL_ELSE, (yyvsp[-1].exp), (yyvsp[0].exp), sourcefilename, yylineno);
		/*Put the else node in the IF node*/
		(yyval.exp) = new OpNode(PEBL_IFELSE, (yyvsp[-4].exp), tmpNode, sourcefilename, yylineno); ;}
    break;

  case 24:
#line 252 "src/base/grammar.y"
    {
		PNode * tmpNode = new OpNode(PEBL_VARIABLEDATUM, (yyvsp[-5].exp), (yyvsp[-3].exp), sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_LOOP, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno); ;}
    break;

  case 25:
#line 259 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_RETURN, (yyvsp[0].exp), NULL, sourcefilename, yylineno);;}
    break;

  case 26:
#line 263 "src/base/grammar.y"
    {

		(yyval.exp) = (yyvsp[0].exp); ;}
    break;

  case 27:
#line 270 "src/base/grammar.y"
    {
		/*First make the else node*/
		  printf("parsing elseif  block .\n");
		  (yyval.exp) =  new OpNode(PEBL_IF, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);

   ;}
    break;

  case 28:
#line 276 "src/base/grammar.y"
    {

		  printf("parsing elseif:elseif statement.\n");
		/*First make the else node*/
		PNode * tmpNode = new OpNode(PEBL_ELSE, (yyvsp[-1].exp), (yyvsp[0].exp), sourcefilename, yylineno);
		/*Put the else node in the IF node*/
		(yyval.exp) = new OpNode(PEBL_IFELSE, (yyvsp[-4].exp), tmpNode, sourcefilename, yylineno); ;}
    break;

  case 29:
#line 294 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_ARGLIST, NULL, NULL, sourcefilename, yylineno);;}
    break;

  case 30:
#line 298 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_ARGLIST, (yyvsp[-1].exp), NULL, sourcefilename, yylineno);;}
    break;

  case 31:
#line 308 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_LISTHEAD,(yyvsp[-1].exp), NULL, sourcefilename, yylineno);;}
    break;

  case 32:
#line 311 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_LISTHEAD, NULL, NULL, sourcefilename, yylineno);;}
    break;

  case 33:
#line 315 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_LISTITEM, (yyvsp[0].exp), NULL, sourcefilename, yylineno);;}
    break;

  case 34:
#line 319 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_LISTITEM, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 35:
#line 327 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_VARLIST, (yyvsp[0].exp), NULL, sourcefilename, yylineno);;}
    break;

  case 36:
#line 331 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_VARLIST,(yyvsp[-3].exp),(yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 37:
#line 337 "src/base/grammar.y"
    { (yyval.exp) = (yyvsp[0].exp);;}
    break;

  case 38:
#line 340 "src/base/grammar.y"
    {(yyval.exp) = (yyvsp[-2].exp);}
    break;

  case 39:
#line 343 "src/base/grammar.y"
    {
		Variant tmpV = 0;
		PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_SUBTRACT, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno); ;}
    break;

  case 40:
#line 349 "src/base/grammar.y"
    {(yyval.exp) = new OpNode(PEBL_NOT, (yyvsp[0].exp), NULL, sourcefilename, yylineno); ;}
    break;

  case 41:
#line 352 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_ADD, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  ;}
    break;

  case 42:
#line 356 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_DIVIDE,(yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  ;}
    break;

  case 43:
#line 359 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_MULTIPLY, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 44:
#line 362 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_POWER, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 45:
#line 365 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_SUBTRACT, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); ;}
    break;

  case 46:
#line 368 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_OR, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); ;}
    break;

  case 47:
#line 371 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_AND, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); ;}
    break;

  case 48:
#line 375 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_LT,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 49:
#line 378 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_GT,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 50:
#line 381 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_GE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 51:
#line 384 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_LE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 52:
#line 387 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_EQ,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 53:
#line 390 "src/base/grammar.y"
    { (yyval.exp) = new OpNode(PEBL_NE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);;}
    break;

  case 54:
#line 399 "src/base/grammar.y"
    {
		PNode * tmpNode = new DataNode(Variant((yyvsp[-1].function), P_DATA_FUNCTION), sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);
        free((yyvsp[-1].function));
		;}
    break;

  case 55:
#line 409 "src/base/grammar.y"
    { (yyval.exp) = new DataNode ((yyvsp[0].iValue), sourcefilename, yylineno); ;}
    break;

  case 56:
#line 412 "src/base/grammar.y"
    { (yyval.exp) = new DataNode ((yyvsp[0].fValue), sourcefilename, yylineno);;}
    break;

  case 57:
#line 415 "src/base/grammar.y"
    {
	  Variant tmpV((yyvsp[0].strValue));            /*create a new temporary variant*/
	  free((yyvsp[0].strValue));
	  (yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);

                        ;}
    break;

  case 58:
#line 422 "src/base/grammar.y"
    { (yyval.exp) = (yyvsp[0].exp);;}
    break;

  case 59:
#line 425 "src/base/grammar.y"
    { (yyval.exp) = (yyvsp[0].exp);;}
    break;

  case 60:
#line 430 "src/base/grammar.y"
    { 
		Variant tmpV((yyvsp[0].symbol), P_DATA_LOCALVARIABLE);           /*create a new temporary variant*/;
		(yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);                        /*Make a new variable node here.*/
        free((yyvsp[0].symbol));
        ;}
    break;

  case 61:
#line 435 "src/base/grammar.y"
    { 
		Variant tmpV((yyvsp[0].symbol), P_DATA_GLOBALVARIABLE);          /*create a new temporary variant*/;
		(yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);  /*Make a new variable node here.*/
		free((yyvsp[0].symbol));
		 ;}
    break;

  case 62:
#line 442 "src/base/grammar.y"
    {/*nothing*/;;}
    break;

  case 63:
#line 443 "src/base/grammar.y"
    {/**/;;}
    break;

  case 64:
#line 447 "src/base/grammar.y"
    {/**/;;}
    break;

  case 65:
#line 448 "src/base/grammar.y"
    {/**/;;}
    break;


    }

/* Line 1037 of yacc.c.  */
#line 1734 "src/base/grammar.tab.cpp"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {

		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 yydestruct ("Error: popping",
                             yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
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
  yydestruct ("Error: discarding lookahead",
              yytoken, &yylval);
  yychar = YYEMPTY;
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 454 "src/base/grammar.y"


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


