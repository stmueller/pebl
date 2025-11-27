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



#line 102 "src/base/grammar.tab.cpp"

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
  YYSYMBOL_elseifseq = 101,                /* elseifseq  */
  YYSYMBOL_arglist = 102,                  /* arglist  */
  YYSYMBOL_list = 103,                     /* list  */
  YYSYMBOL_explist = 104,                  /* explist  */
  YYSYMBOL_varlist = 105,                  /* varlist  */
  YYSYMBOL_variablepair = 106,             /* variablepair  */
  YYSYMBOL_exp = 107,                      /* exp  */
  YYSYMBOL_datum = 108,                    /* datum  */
  YYSYMBOL_variable = 109,                 /* variable  */
  YYSYMBOL_nlornone = 110,                 /* nlornone  */
  YYSYMBOL_newlines = 111                  /* newlines  */
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
#define YYNTOKENS  90
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  163

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
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   163,   163,   171,   179,   189,   204,   213,   217,   218,
     222,   223,   224,   226,   232,   235,   239,   243,   252,   254,
     256,   265,   273,   286,   292,   308,   315,   320,   327,   333,
     350,   354,   364,   367,   371,   375,   383,   387,   394,   396,
     404,   407,   410,   416,   419,   423,   426,   429,   432,   435,
     438,   442,   445,   448,   451,   454,   457,   466,   480,   483,
     486,   493,   496,   501,   506,   515,   516,   520,   521
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

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
  "endstatement", "ustatement", "returnstatement", "elseifseq", "arglist",
  "list", "explist", "varlist", "variablepair", "exp", "datum", "variable",
  "nlornone", "newlines", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-114)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -25,  -114,    11,    15,     4,  -114,   -42,   -25,   -25,  -114,
      17,  -114,  -114,   -37,   -25,  -114,  -114,    16,    51,     5,
      37,   -25,   -25,   177,   -25,  -114,  -114,    37,   -74,   -25,
     -25,   177,   177,  -114,  -114,  -114,    32,  -114,   447,  -114,
    -114,   103,  -114,  -114,    -6,   177,   467,    26,   -26,  -114,
     -25,   -25,   -25,   177,   177,   177,   177,   177,   -25,   177,
     -25,   -25,   -25,    31,    39,    40,   262,  -114,    42,    62,
      69,    43,   -25,  -114,  -114,   -14,   -25,   447,  -114,   -25,
     291,   337,  -114,   177,   177,   177,   177,    24,    24,    24,
      24,    24,   177,    24,   177,   177,   177,  -114,   177,   -74,
    -114,    44,   177,   177,   177,  -114,   167,  -114,  -114,  -114,
      45,   -25,    46,   -25,   467,    -2,    26,    26,   467,  -114,
      -2,   357,    76,   382,   447,   447,  -114,  -114,  -114,   -25,
    -114,   177,  -114,    48,   -25,   177,   -25,  -114,  -114,  -114,
      61,   402,    61,   -25,    20,   -25,  -114,   194,   -25,    60,
    -114,    61,   -25,    61,   177,  -114,   202,  -114,   427,   -25,
      61,    20,  -114
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
    -114,  -114,    90,    67,  -114,  -113,   -50,   -64,  -100,    33,
      -8,   -61,  -114,  -114,   -79,    73,  -114,    64,  -114,   -10,
      -7,  -114
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     7,    25,    71,    26,    72,    73,    74,    75,
      76,   150,    49,    37,    79,    17,    18,    77,    39,    40,
       3,     4
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      11,    12,   100,    19,   113,    14,   128,    20,    52,     1,
       1,     5,    15,    16,    27,    28,    82,    41,    19,    29,
       6,   107,    44,    45,     6,    30,   108,   144,    51,   146,
      31,    58,   148,   149,    52,    78,    61,    32,   155,     9,
     157,    83,   127,    84,    85,    86,    10,   161,    13,    15,
      16,    92,   138,    94,    95,    96,   128,    58,    21,    22,
      23,    24,    61,    48,    61,   106,    97,    62,   103,   109,
      98,    99,   110,   102,   112,   104,    33,    34,    35,   107,
      15,    16,    36,   105,   135,   143,   130,    38,   132,   122,
     139,   154,   127,     8,    42,    46,    47,   152,   129,   101,
     162,    43,     0,     0,   131,     0,   133,     0,    80,    81,
      63,     0,     0,     0,     0,     0,     0,    87,    88,    89,
      90,    91,   137,    93,    64,     0,     0,   140,    29,   142,
       0,     0,     0,    65,    30,     0,   147,     0,   151,    31,
       0,   153,    66,    67,     0,   156,    32,    80,   114,   115,
     116,     0,   160,    68,     0,     0,   117,     0,   118,   119,
     120,     0,   121,     0,     0,     0,   123,   124,   125,     0,
       0,     0,     0,     0,    63,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,    34,    35,    64,    69,
      70,    36,    29,     0,     0,    80,     0,    65,    30,   141,
       0,    63,    29,    31,     0,     0,    66,   126,    30,    63,
      32,     0,     0,    31,     0,    64,     0,    68,   158,    29,
      32,     0,     0,    64,    65,    30,     0,    29,     0,     0,
      31,     0,    65,    30,    67,     0,     0,    32,    31,     0,
       0,     0,   126,     0,    68,    32,     0,     0,     0,    33,
      34,    35,    68,    69,    70,    36,     0,     0,     0,    33,
      34,    35,     0,    15,    16,    36,     0,     0,     0,    63,
       0,     0,     0,     0,     0,     0,    33,    34,    35,     0,
      69,    70,    36,    64,    33,    34,    35,    29,    69,    70,
      36,     0,    65,    30,    50,    51,     0,     0,    31,   111,
       0,    52,     0,     0,     0,    32,     0,    53,     0,     0,
      54,    55,    68,     0,     0,     0,     0,    56,     0,     0,
       0,     0,     0,    57,    58,    59,     0,     0,    60,    61,
       0,     0,     0,     0,    62,     0,     0,     0,     0,     0,
      50,    51,     0,     0,    33,    34,    35,    52,    69,    70,
      36,     0,     0,    53,     0,     0,    54,    55,     0,     0,
      50,    51,     0,    56,     0,     0,     0,    52,     0,    57,
      58,    59,     1,    53,    60,    61,    54,    55,     0,     0,
      62,     0,     0,    56,     0,    50,    51,     0,     0,    57,
      58,    59,    52,     0,    60,    61,     0,     0,    53,   134,
      62,    54,    55,     0,     0,    50,    51,     0,    56,     0,
       0,     0,    52,     0,    57,    58,    59,     0,    53,    60,
      61,    54,    55,     0,   136,    62,     0,     0,    56,     0,
      50,    51,     0,     0,    57,    58,    59,    52,     0,    60,
      61,     0,     0,    53,   145,    62,    54,    55,     0,     0,
      50,    51,     0,    56,     0,     0,     0,    52,     0,    57,
      58,    59,     0,    53,    60,    61,    54,    55,     0,   159,
      62,    51,     0,    56,     0,     0,     0,    52,     0,    57,
      58,    59,     0,    53,    60,    61,    54,    55,     0,     0,
      62,     0,     0,    56,     0,     0,     0,     0,     0,    57,
      58,    59,     0,     0,     0,    61,     0,     0,     0,     0,
      62
};

static const yytype_int16 yycheck[] =
{
       7,     8,    66,    13,    83,    42,   106,    14,    10,    35,
      35,     0,    86,    87,    21,    22,    42,    24,    28,    25,
       9,    35,    29,    30,     9,    31,    40,   140,     4,   142,
      36,    33,    12,    13,    10,    41,    38,    43,   151,    35,
     153,    48,   106,    50,    51,    52,    88,   160,    31,    86,
      87,    58,   131,    60,    61,    62,   156,    33,    42,     8,
      55,    24,    38,    31,    38,    72,    35,    43,     6,    76,
      31,    31,    79,    31,    81,     6,    82,    83,    84,    35,
      86,    87,    88,    40,     8,    24,    41,    23,    42,    99,
      42,    31,   156,     3,    27,    31,    32,   147,   106,    66,
     161,    28,    -1,    -1,   111,    -1,   113,    -1,    44,    45,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,
      56,    57,   129,    59,    21,    -1,    -1,   134,    25,   136,
      -1,    -1,    -1,    30,    31,    -1,   143,    -1,   145,    36,
      -1,   148,    39,    40,    -1,   152,    43,    83,    84,    85,
      86,    -1,   159,    50,    -1,    -1,    92,    -1,    94,    95,
      96,    -1,    98,    -1,    -1,    -1,   102,   103,   104,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    83,    84,    21,    86,
      87,    88,    25,    -1,    -1,   131,    -1,    30,    31,   135,
      -1,     7,    25,    36,    -1,    -1,    39,    40,    31,     7,
      43,    -1,    -1,    36,    -1,    21,    -1,    50,   154,    25,
      43,    -1,    -1,    21,    30,    31,    -1,    25,    -1,    -1,
      36,    -1,    30,    31,    40,    -1,    -1,    43,    36,    -1,
      -1,    -1,    40,    -1,    50,    43,    -1,    -1,    -1,    82,
      83,    84,    50,    86,    87,    88,    -1,    -1,    -1,    82,
      83,    84,    -1,    86,    87,    88,    -1,    -1,    -1,     7,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    83,    84,    -1,
      86,    87,    88,    21,    82,    83,    84,    25,    86,    87,
      88,    -1,    30,    31,     3,     4,    -1,    -1,    36,     8,
      -1,    10,    -1,    -1,    -1,    43,    -1,    16,    -1,    -1,
      19,    20,    50,    -1,    -1,    -1,    -1,    26,    -1,    -1,
      -1,    -1,    -1,    32,    33,    34,    -1,    -1,    37,    38,
      -1,    -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,
       3,     4,    -1,    -1,    82,    83,    84,    10,    86,    87,
      88,    -1,    -1,    16,    -1,    -1,    19,    20,    -1,    -1,
       3,     4,    -1,    26,    -1,    -1,    -1,    10,    -1,    32,
      33,    34,    35,    16,    37,    38,    19,    20,    -1,    -1,
      43,    -1,    -1,    26,    -1,     3,     4,    -1,    -1,    32,
      33,    34,    10,    -1,    37,    38,    -1,    -1,    16,    42,
      43,    19,    20,    -1,    -1,     3,     4,    -1,    26,    -1,
      -1,    -1,    10,    -1,    32,    33,    34,    -1,    16,    37,
      38,    19,    20,    -1,    42,    43,    -1,    -1,    26,    -1,
       3,     4,    -1,    -1,    32,    33,    34,    10,    -1,    37,
      38,    -1,    -1,    16,    42,    43,    19,    20,    -1,    -1,
       3,     4,    -1,    26,    -1,    -1,    -1,    10,    -1,    32,
      33,    34,    -1,    16,    37,    38,    19,    20,    -1,    42,
      43,     4,    -1,    26,    -1,    -1,    -1,    10,    -1,    32,
      33,    34,    -1,    16,    37,    38,    19,    20,    -1,    -1,
      43,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    32,
      33,    34,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      43
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    35,    91,   110,   111,     0,     9,    92,    92,    35,
      88,   110,   110,    31,    42,    86,    87,   105,   106,   109,
     110,    42,     8,    55,    24,    93,    95,   110,   110,    25,
      31,    36,    43,    82,    83,    84,    88,   103,   107,   108,
     109,   110,    93,   105,   110,   110,   107,   107,    31,   102,
       3,     4,    10,    16,    19,    20,    26,    32,    33,    34,
      37,    38,    43,     7,    21,    30,    39,    40,    50,    86,
      87,    94,    96,    97,    98,    99,   100,   107,    41,   104,
     107,   107,    42,   110,   110,   110,   110,   107,   107,   107,
     107,   107,   110,   107,   110,   110,   110,    35,    31,    31,
      97,    99,    31,     6,     6,    40,   110,    35,    40,   110,
     110,     8,   110,   104,   107,   107,   107,   107,   107,   107,
     107,   107,   109,   107,   107,   107,    40,    97,    98,   100,
      41,   110,    42,   110,    42,     8,    42,   110,   104,    42,
     110,   107,   110,    24,    95,    42,    95,   110,    12,    13,
     101,   110,    96,   110,    31,    95,   110,    95,   107,    42,
     110,    95,   101
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    90,    91,    91,    92,    92,    93,    93,    94,    94,
      95,    95,    95,    95,    96,    96,    97,    98,    99,    99,
      99,    99,    99,    99,    99,    99,   100,   101,   101,   101,
     102,   102,   103,   103,   104,   104,   105,   105,   106,   106,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   108,   108,
     108,   108,   108,   109,   109,   110,   110,   111,   111
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
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


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
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
                 int yyrule)
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
                       &yyvsp[(yyi + 1) - (yynrhs)]);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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
yyparse (void)
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
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
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
  case 2: /* functions: nlornone function nlornone  */
#line 163 "src/base/grammar.y"
                                           {gParseTreeHead =  new OpNode(PEBL_FUNCTIONS,(yyvsp[-1].exp),NULL, sourcefilename, yylineno);

		           (yyval.exp) = gParseTreeHead;
		}
#line 1395 "src/base/grammar.tab.cpp"
    break;

  case 3: /* functions: functions function nlornone  */
#line 171 "src/base/grammar.y"
                                            { gParseTreeHead = new OpNode(PEBL_FUNCTIONS, (yyvsp[-1].exp), (yyvsp[-2].exp), sourcefilename, yylineno);
		           (yyval.exp) = gParseTreeHead;
		}
#line 1403 "src/base/grammar.tab.cpp"
    break;

  case 4: /* function: PEBL_DEFINE PEBL_FUNCTIONNAME PEBL_LPAREN varlist PEBL_RPAREN nlornone functionblock  */
#line 179 "src/base/grammar.y"
                                                                                                       { ;
		PNode * tmpFN = new OpNode(PEBL_LAMBDAFUNCTION, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  
		tmpFN->SetFunctionName((yyvsp[-5].function));
		PNode * tmpNode = new DataNode(Variant((yyvsp[-5].function), P_DATA_FUNCTION), sourcefilename, yylineno);
		free((yyvsp[-5].function));
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, tmpFN, sourcefilename, yylineno);

        }
#line 1416 "src/base/grammar.tab.cpp"
    break;

  case 5: /* function: PEBL_DEFINE PEBL_FUNCTIONNAME PEBL_LPAREN PEBL_RPAREN nlornone functionblock  */
#line 189 "src/base/grammar.y"
                                                                                                 { ;
		PNode * tmpFN = new OpNode(PEBL_LAMBDAFUNCTION, NULL, (yyvsp[0].exp), sourcefilename, yylineno); 
		tmpFN->SetFunctionName((yyvsp[-4].function));
		PNode * tmpNode = new DataNode(Variant((yyvsp[-4].function), P_DATA_FUNCTION), sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, tmpFN, sourcefilename, yylineno);
                free((yyvsp[-4].function));
		  }
#line 1428 "src/base/grammar.tab.cpp"
    break;

  case 6: /* functionblock: block  */
#line 204 "src/base/grammar.y"
                      {
		   /*When no return value is provided, return 1 (true)*/
                  DataNode* retval  = new DataNode (Variant(1), sourcefilename, yylineno);
		  OpNode *tmpReturn = new OpNode(PEBL_RETURN, retval, NULL, sourcefilename, yylineno);
    	           (yyval.exp) = new OpNode(PEBL_STATEMENTS,(yyvsp[0].exp),tmpReturn,sourcefilename,yylineno);
                 }
#line 1439 "src/base/grammar.tab.cpp"
    break;

  case 7: /* functionblock: PEBL_LBRACE nlornone functionsequence PEBL_RBRACE  */
#line 213 "src/base/grammar.y"
                                                                    {(yyval.exp) = (yyvsp[-1].exp);}
#line 1445 "src/base/grammar.tab.cpp"
    break;

  case 8: /* functionsequence: returnstatement nlornone  */
#line 217 "src/base/grammar.y"
                                                       { (yyval.exp) = (yyvsp[-1].exp);}
#line 1451 "src/base/grammar.tab.cpp"
    break;

  case 9: /* functionsequence: sequence nlornone returnstatement nlornone  */
#line 218 "src/base/grammar.y"
                                                                { (yyval.exp) = new OpNode(PEBL_STATEMENTS, (yyvsp[-3].exp), (yyvsp[-1].exp), sourcefilename, yylineno);}
#line 1457 "src/base/grammar.tab.cpp"
    break;

  case 10: /* block: PEBL_LBRACE nlornone sequence nlornone PEBL_RBRACE  */
#line 222 "src/base/grammar.y"
                                                                     { (yyval.exp) = (yyvsp[-2].exp);}
#line 1463 "src/base/grammar.tab.cpp"
    break;

  case 11: /* block: PEBL_LBRACE nlornone endstatement  */
#line 223 "src/base/grammar.y"
                                              {(yyval.exp) = (yyvsp[0].exp);}
#line 1469 "src/base/grammar.tab.cpp"
    break;

  case 12: /* block: PEBL_LBRACE nlornone sequence nlornone endstatement  */
#line 224 "src/base/grammar.y"
                                                                 {
  (yyval.exp)  = new OpNode(PEBL_STATEMENTS, (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1476 "src/base/grammar.tab.cpp"
    break;

  case 13: /* block: PEBL_LBRACE nlornone PEBL_RBRACE  */
#line 226 "src/base/grammar.y"
                                         { (yyval.exp) = new DataNode (Variant(0), sourcefilename, yylineno);}
#line 1482 "src/base/grammar.tab.cpp"
    break;

  case 14: /* sequence: statement  */
#line 232 "src/base/grammar.y"
                                       { (yyval.exp) = (yyvsp[0].exp); }
#line 1488 "src/base/grammar.tab.cpp"
    break;

  case 15: /* sequence: sequence nlornone statement  */
#line 235 "src/base/grammar.y"
                                                   { (yyval.exp) = new OpNode(PEBL_STATEMENTS, (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1494 "src/base/grammar.tab.cpp"
    break;

  case 16: /* statement: ustatement PEBL_NEWLINE  */
#line 239 "src/base/grammar.y"
                                   {(yyval.exp) = (yyvsp[-1].exp);}
#line 1500 "src/base/grammar.tab.cpp"
    break;

  case 17: /* endstatement: ustatement PEBL_RBRACE  */
#line 243 "src/base/grammar.y"
                                     {(yyval.exp) = (yyvsp[-1].exp);}
#line 1506 "src/base/grammar.tab.cpp"
    break;

  case 18: /* ustatement: exp  */
#line 252 "src/base/grammar.y"
                                      {(yyval.exp) = (yyvsp[0].exp);}
#line 1512 "src/base/grammar.tab.cpp"
    break;

  case 19: /* ustatement: PEBL_BREAK PEBL_NEWLINE  */
#line 254 "src/base/grammar.y"
                                          {(yyval.exp) = new OpNode(PEBL_BREAK, NULL, NULL, sourcefilename, yylineno);}
#line 1518 "src/base/grammar.tab.cpp"
    break;

  case 20: /* ustatement: PEBL_LOCALVAR PEBL_ASSIGN exp  */
#line 257 "src/base/grammar.y"
                { 
	         Variant tmpV((yyvsp[-2].symbol),P_DATA_LOCALVARIABLE);       /*create a new temporary variant*/
		 /*free($1);*/
		 PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);        /*create basic pnode*/
		 (yyval.exp) = new OpNode(PEBL_ASSIGN, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);   /*Use symbol node in assignment node*/
		}
#line 1529 "src/base/grammar.tab.cpp"
    break;

  case 21: /* ustatement: PEBL_GLOBALVAR PEBL_ASSIGN exp  */
#line 266 "src/base/grammar.y"
                { 
	        Variant tmpV((yyvsp[-2].symbol),P_DATA_GLOBALVARIABLE);      /*create a new temporary variant*/
		PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);        /*create basic pnode*/
		(yyval.exp) = new OpNode(PEBL_ASSIGN, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);   /*Use symbol node in assignment node*/
		}
#line 1539 "src/base/grammar.tab.cpp"
    break;

  case 22: /* ustatement: PEBL_WHILE PEBL_LPAREN exp PEBL_RPAREN nlornone block  */
#line 273 "src/base/grammar.y"
                                                                       {;
		(yyval.exp) = new OpNode(PEBL_WHILE, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1546 "src/base/grammar.tab.cpp"
    break;

  case 23: /* ustatement: PEBL_IF PEBL_LPAREN exp PEBL_RPAREN nlornone block  */
#line 286 "src/base/grammar.y"
                                                                    {
		(yyval.exp) = new OpNode(PEBL_IF, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1553 "src/base/grammar.tab.cpp"
    break;

  case 24: /* ustatement: PEBL_IF PEBL_LPAREN exp PEBL_RPAREN nlornone block elseifseq  */
#line 292 "src/base/grammar.y"
                                                                             {
		/*First make the else node*/
		PNode * tmpNode = new OpNode(PEBL_ELSE, (yyvsp[-1].exp), (yyvsp[0].exp), sourcefilename, yylineno);
		/*Put the else node in the IF node*/
		(yyval.exp) = new OpNode(PEBL_IFELSE, (yyvsp[-4].exp), tmpNode, sourcefilename, yylineno); }
#line 1563 "src/base/grammar.tab.cpp"
    break;

  case 25: /* ustatement: PEBL_LOOP PEBL_LPAREN variable PEBL_COMMA exp PEBL_RPAREN nlornone block  */
#line 308 "src/base/grammar.y"
                                                                                          {
		PNode * tmpNode = new OpNode(PEBL_VARIABLEDATUM, (yyvsp[-5].exp), (yyvsp[-3].exp), sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_LOOP, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1571 "src/base/grammar.tab.cpp"
    break;

  case 26: /* returnstatement: PEBL_RETURN statement  */
#line 315 "src/base/grammar.y"
                                          {(yyval.exp) = new OpNode(PEBL_RETURN, (yyvsp[0].exp), NULL, sourcefilename, yylineno);}
#line 1577 "src/base/grammar.tab.cpp"
    break;

  case 27: /* elseifseq: PEBL_ELSE nlornone block  */
#line 320 "src/base/grammar.y"
                                      {

		(yyval.exp) = (yyvsp[0].exp); }
#line 1585 "src/base/grammar.tab.cpp"
    break;

  case 28: /* elseifseq: PEBL_ELSEIF PEBL_LPAREN exp PEBL_RPAREN nlornone block  */
#line 327 "src/base/grammar.y"
                                                                {
		/*First make the else node*/

		  (yyval.exp) =  new OpNode(PEBL_IF, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);

   }
#line 1596 "src/base/grammar.tab.cpp"
    break;

  case 29: /* elseifseq: PEBL_ELSEIF PEBL_LPAREN exp PEBL_RPAREN nlornone block elseifseq  */
#line 333 "src/base/grammar.y"
                                                                         {

		/*First make the else node*/
		PNode * tmpNode = new OpNode(PEBL_ELSE, (yyvsp[-1].exp), (yyvsp[0].exp), sourcefilename, yylineno);
		/*Put the else node in the IF node*/
		(yyval.exp) = new OpNode(PEBL_IFELSE, (yyvsp[-4].exp), tmpNode, sourcefilename, yylineno); }
#line 1607 "src/base/grammar.tab.cpp"
    break;

  case 30: /* arglist: PEBL_LPAREN PEBL_RPAREN  */
#line 350 "src/base/grammar.y"
                                                            {(yyval.exp) = new OpNode(PEBL_ARGLIST, NULL, NULL, sourcefilename, yylineno);}
#line 1613 "src/base/grammar.tab.cpp"
    break;

  case 31: /* arglist: PEBL_LPAREN nlornone explist nlornone PEBL_RPAREN  */
#line 354 "src/base/grammar.y"
                                                                     {(yyval.exp) = new OpNode(PEBL_ARGLIST, (yyvsp[-2].exp), NULL, sourcefilename, yylineno);}
#line 1619 "src/base/grammar.tab.cpp"
    break;

  case 32: /* list: PEBL_LBRACKET nlornone explist nlornone PEBL_RBRACKET  */
#line 364 "src/base/grammar.y"
                                                                      {(yyval.exp) = new OpNode(PEBL_LISTHEAD,(yyvsp[-2].exp), NULL, sourcefilename, yylineno);}
#line 1625 "src/base/grammar.tab.cpp"
    break;

  case 33: /* list: PEBL_LBRACKET nlornone PEBL_RBRACKET  */
#line 367 "src/base/grammar.y"
                                                               {(yyval.exp) = new OpNode(PEBL_LISTHEAD, NULL, NULL, sourcefilename, yylineno);}
#line 1631 "src/base/grammar.tab.cpp"
    break;

  case 34: /* explist: exp  */
#line 371 "src/base/grammar.y"
                                                      {(yyval.exp) = new OpNode(PEBL_LISTITEM, (yyvsp[0].exp), NULL, sourcefilename, yylineno);}
#line 1637 "src/base/grammar.tab.cpp"
    break;

  case 35: /* explist: exp PEBL_COMMA nlornone explist  */
#line 375 "src/base/grammar.y"
                                                      {(yyval.exp) = new OpNode(PEBL_LISTITEM, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1643 "src/base/grammar.tab.cpp"
    break;

  case 36: /* varlist: variablepair  */
#line 383 "src/base/grammar.y"
                                  {(yyval.exp) = new OpNode(PEBL_VARLIST, (yyvsp[0].exp), NULL, sourcefilename, yylineno);}
#line 1649 "src/base/grammar.tab.cpp"
    break;

  case 37: /* varlist: variablepair PEBL_COMMA nlornone varlist  */
#line 387 "src/base/grammar.y"
                                                           {(yyval.exp) = new OpNode(PEBL_VARLIST,(yyvsp[-3].exp),(yyvsp[0].exp), sourcefilename, yylineno);}
#line 1655 "src/base/grammar.tab.cpp"
    break;

  case 38: /* variablepair: variable  */
#line 394 "src/base/grammar.y"
                         {(yyval.exp) = (yyvsp[0].exp);}
#line 1661 "src/base/grammar.tab.cpp"
    break;

  case 39: /* variablepair: variable PEBL_COLON exp  */
#line 396 "src/base/grammar.y"
                                       {
		       (yyval.exp)  = new OpNode(PEBL_VARPAIR,(yyvsp[-2].exp), (yyvsp[0].exp),sourcefilename,yylineno);
	       }
#line 1669 "src/base/grammar.tab.cpp"
    break;

  case 40: /* exp: datum  */
#line 404 "src/base/grammar.y"
                                        { (yyval.exp) = (yyvsp[0].exp);}
#line 1675 "src/base/grammar.tab.cpp"
    break;

  case 41: /* exp: PEBL_LPAREN nlornone exp nlornone PEBL_RPAREN  */
#line 407 "src/base/grammar.y"
                                                                 {(yyval.exp) = (yyvsp[-2].exp);}
#line 1681 "src/base/grammar.tab.cpp"
    break;

  case 42: /* exp: PEBL_SUBTRACT exp  */
#line 410 "src/base/grammar.y"
                                                        {
		Variant tmpV = 0;
		PNode * tmpNode = new DataNode(tmpV, sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_SUBTRACT, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1690 "src/base/grammar.tab.cpp"
    break;

  case 43: /* exp: PEBL_NOT exp  */
#line 416 "src/base/grammar.y"
                                                    {(yyval.exp) = new OpNode(PEBL_NOT, (yyvsp[0].exp), NULL, sourcefilename, yylineno); }
#line 1696 "src/base/grammar.tab.cpp"
    break;

  case 44: /* exp: exp PEBL_ADD nlornone exp  */
#line 419 "src/base/grammar.y"
                                                  { (yyval.exp) = new OpNode(PEBL_ADD, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  }
#line 1702 "src/base/grammar.tab.cpp"
    break;

  case 45: /* exp: exp PEBL_DIVIDE nlornone exp  */
#line 423 "src/base/grammar.y"
                                                   { (yyval.exp) = new OpNode(PEBL_DIVIDE,(yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);  }
#line 1708 "src/base/grammar.tab.cpp"
    break;

  case 46: /* exp: exp PEBL_MULTIPLY nlornone exp  */
#line 426 "src/base/grammar.y"
                                                  { (yyval.exp) = new OpNode(PEBL_MULTIPLY, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1714 "src/base/grammar.tab.cpp"
    break;

  case 47: /* exp: exp PEBL_POWER nlornone exp  */
#line 429 "src/base/grammar.y"
                                               { (yyval.exp) = new OpNode(PEBL_POWER, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1720 "src/base/grammar.tab.cpp"
    break;

  case 48: /* exp: exp PEBL_SUBTRACT nlornone exp  */
#line 432 "src/base/grammar.y"
                                                  { (yyval.exp) = new OpNode(PEBL_SUBTRACT, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1726 "src/base/grammar.tab.cpp"
    break;

  case 49: /* exp: exp PEBL_OR nlornone exp  */
#line 435 "src/base/grammar.y"
                                            { (yyval.exp) = new OpNode(PEBL_OR, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1732 "src/base/grammar.tab.cpp"
    break;

  case 50: /* exp: exp PEBL_AND nlornone exp  */
#line 438 "src/base/grammar.y"
                                            { (yyval.exp) = new OpNode(PEBL_AND, (yyvsp[-3].exp), (yyvsp[0].exp), sourcefilename, yylineno); }
#line 1738 "src/base/grammar.tab.cpp"
    break;

  case 51: /* exp: exp PEBL_LT exp  */
#line 442 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_LT,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1744 "src/base/grammar.tab.cpp"
    break;

  case 52: /* exp: exp PEBL_GT exp  */
#line 445 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_GT,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1750 "src/base/grammar.tab.cpp"
    break;

  case 53: /* exp: exp PEBL_GE exp  */
#line 448 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_GE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1756 "src/base/grammar.tab.cpp"
    break;

  case 54: /* exp: exp PEBL_LE exp  */
#line 451 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_LE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1762 "src/base/grammar.tab.cpp"
    break;

  case 55: /* exp: exp PEBL_EQ exp  */
#line 454 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_EQ,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1768 "src/base/grammar.tab.cpp"
    break;

  case 56: /* exp: exp PEBL_NE exp  */
#line 457 "src/base/grammar.y"
                                         { (yyval.exp) = new OpNode(PEBL_NE,  (yyvsp[-2].exp), (yyvsp[0].exp), sourcefilename, yylineno);}
#line 1774 "src/base/grammar.tab.cpp"
    break;

  case 57: /* exp: PEBL_FUNCTIONNAME arglist  */
#line 466 "src/base/grammar.y"
                                          {


		  /*Memory leak here, by electricfence*/
		Variant v = Variant((yyvsp[-1].function),P_DATA_FUNCTION);
		PNode * tmpNode = new DataNode(v, sourcefilename, yylineno);
		(yyval.exp) = new OpNode(PEBL_FUNCTION, tmpNode, (yyvsp[0].exp), sourcefilename, yylineno);
		free((yyvsp[-1].function));
		}
#line 1788 "src/base/grammar.tab.cpp"
    break;

  case 58: /* datum: PEBL_INTEGER  */
#line 480 "src/base/grammar.y"
                                         { (yyval.exp) = new DataNode ((yyvsp[0].iValue), sourcefilename, yylineno); }
#line 1794 "src/base/grammar.tab.cpp"
    break;

  case 59: /* datum: PEBL_FLOAT  */
#line 483 "src/base/grammar.y"
                                         { (yyval.exp) = new DataNode ((yyvsp[0].fValue), sourcefilename, yylineno);}
#line 1800 "src/base/grammar.tab.cpp"
    break;

  case 60: /* datum: PEBL_STRING  */
#line 486 "src/base/grammar.y"
                                         {
	  Variant tmpV((yyvsp[0].strValue));            /*create a new temporary variant*/
	  free((yyvsp[0].strValue));
	  (yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);

                        }
#line 1811 "src/base/grammar.tab.cpp"
    break;

  case 61: /* datum: list  */
#line 493 "src/base/grammar.y"
                                         { (yyval.exp) = (yyvsp[0].exp);}
#line 1817 "src/base/grammar.tab.cpp"
    break;

  case 62: /* datum: variable  */
#line 496 "src/base/grammar.y"
                                         { (yyval.exp) = (yyvsp[0].exp);}
#line 1823 "src/base/grammar.tab.cpp"
    break;

  case 63: /* variable: PEBL_LOCALVAR  */
#line 501 "src/base/grammar.y"
                                           { 
		Variant tmpV((yyvsp[0].symbol), P_DATA_LOCALVARIABLE);           /*create a new temporary variant*/;
		(yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);                        /*Make a new variable node here.*/
                free((yyvsp[0].symbol));
                }
#line 1833 "src/base/grammar.tab.cpp"
    break;

  case 64: /* variable: PEBL_GLOBALVAR  */
#line 506 "src/base/grammar.y"
                                            { 
		Variant tmpV((yyvsp[0].symbol), P_DATA_GLOBALVARIABLE);          /*create a new temporary variant*/;
		(yyval.exp) = new DataNode(tmpV, sourcefilename, yylineno);  /*Make a new variable node here.*/
		free((yyvsp[0].symbol));
		 }
#line 1843 "src/base/grammar.tab.cpp"
    break;

  case 65: /* nlornone: %empty  */
#line 515 "src/base/grammar.y"
                       {/*nothing*/;}
#line 1849 "src/base/grammar.tab.cpp"
    break;

  case 66: /* nlornone: newlines  */
#line 516 "src/base/grammar.y"
                                 {/**/;}
#line 1855 "src/base/grammar.tab.cpp"
    break;

  case 67: /* newlines: PEBL_NEWLINE  */
#line 520 "src/base/grammar.y"
                                      {/**/;}
#line 1861 "src/base/grammar.tab.cpp"
    break;

  case 68: /* newlines: newlines PEBL_NEWLINE  */
#line 521 "src/base/grammar.y"
                                      {/**/;}
#line 1867 "src/base/grammar.tab.cpp"
    break;


#line 1871 "src/base/grammar.tab.cpp"

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
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
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 527 "src/base/grammar.y"


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
