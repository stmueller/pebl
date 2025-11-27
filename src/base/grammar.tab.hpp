/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Skeleton interface for Bison GLR parsers in C

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

#ifndef YY_YY_SRC_BASE_GRAMMAR_TAB_HPP_INCLUDED
# define YY_YY_SRC_BASE_GRAMMAR_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    PEBL_AND = 258,                /* PEBL_AND  */
    PEBL_ADD = 259,                /* PEBL_ADD  */
    PEBL_ARGLIST = 260,            /* PEBL_ARGLIST  */
    PEBL_ASSIGN = 261,             /* PEBL_ASSIGN  */
    PEBL_BREAK = 262,              /* PEBL_BREAK  */
    PEBL_COMMA = 263,              /* PEBL_COMMA  */
    PEBL_DEFINE = 264,             /* PEBL_DEFINE  */
    PEBL_DIVIDE = 265,             /* PEBL_DIVIDE  */
    PEBL_DOT = 266,                /* PEBL_DOT  */
    PEBL_ELSE = 267,               /* PEBL_ELSE  */
    PEBL_ELSEIF = 268,             /* PEBL_ELSEIF  */
    PEBL_END = 269,                /* PEBL_END  */
    PEBL_EOF = 270,                /* PEBL_EOF  */
    PEBL_EQ = 271,                 /* PEBL_EQ  */
    PEBL_FUNCTION = 272,           /* PEBL_FUNCTION  */
    PEBL_FUNCTIONS = 273,          /* PEBL_FUNCTIONS  */
    PEBL_GE = 274,                 /* PEBL_GE  */
    PEBL_GT = 275,                 /* PEBL_GT  */
    PEBL_IF = 276,                 /* PEBL_IF  */
    PEBL_IFELSE = 277,             /* PEBL_IFELSE  */
    PEBL_LAMBDAFUNCTION = 278,     /* PEBL_LAMBDAFUNCTION  */
    PEBL_LBRACE = 279,             /* PEBL_LBRACE  */
    PEBL_LBRACKET = 280,           /* PEBL_LBRACKET  */
    PEBL_LE = 281,                 /* PEBL_LE  */
    PEBL_LIBRARYFUNCTION = 282,    /* PEBL_LIBRARYFUNCTION  */
    PEBL_LISTHEAD = 283,           /* PEBL_LISTHEAD  */
    PEBL_LISTITEM = 284,           /* PEBL_LISTITEM  */
    PEBL_LOOP = 285,               /* PEBL_LOOP  */
    PEBL_LPAREN = 286,             /* PEBL_LPAREN  */
    PEBL_LT = 287,                 /* PEBL_LT  */
    PEBL_MULTIPLY = 288,           /* PEBL_MULTIPLY  */
    PEBL_NE = 289,                 /* PEBL_NE  */
    PEBL_NEWLINE = 290,            /* PEBL_NEWLINE  */
    PEBL_NOT = 291,                /* PEBL_NOT  */
    PEBL_OR = 292,                 /* PEBL_OR  */
    PEBL_POWER = 293,              /* PEBL_POWER  */
    PEBL_RETURN = 294,             /* PEBL_RETURN  */
    PEBL_RBRACE = 295,             /* PEBL_RBRACE  */
    PEBL_RBRACKET = 296,           /* PEBL_RBRACKET  */
    PEBL_RPAREN = 297,             /* PEBL_RPAREN  */
    PEBL_SUBTRACT = 298,           /* PEBL_SUBTRACT  */
    PEBL_SEMI = 299,               /* PEBL_SEMI  */
    PEBL_STATEMENTS = 300,         /* PEBL_STATEMENTS  */
    PEBL_START = 301,              /* PEBL_START  */
    PEBL_VARPAIR = 302,            /* PEBL_VARPAIR  */
    PEBL_VARIABLEDATUM = 303,      /* PEBL_VARIABLEDATUM  */
    PEBL_VARLIST = 304,            /* PEBL_VARLIST  */
    PEBL_WHILE = 305,              /* PEBL_WHILE  */
    PEBL_AND_TAIL = 306,           /* PEBL_AND_TAIL  */
    PEBL_ADD_TAIL = 307,           /* PEBL_ADD_TAIL  */
    PEBL_ASSIGN_TAIL = 308,        /* PEBL_ASSIGN_TAIL  */
    PEBL_BREAK_TAIL = 309,         /* PEBL_BREAK_TAIL  */
    PEBL_COLON = 310,              /* PEBL_COLON  */
    PEBL_DIVIDE_TAIL = 311,        /* PEBL_DIVIDE_TAIL  */
    PEBL_EQ_TAIL = 312,            /* PEBL_EQ_TAIL  */
    PEBL_GE_TAIL = 313,            /* PEBL_GE_TAIL  */
    PEBL_GT_TAIL = 314,            /* PEBL_GT_TAIL  */
    PEBL_IF_TAIL = 315,            /* PEBL_IF_TAIL  */
    PEBL_IF_TAIL2 = 316,           /* PEBL_IF_TAIL2  */
    PEBL_ELSE_TAIL = 317,          /* PEBL_ELSE_TAIL  */
    PEBL_LE_TAIL = 318,            /* PEBL_LE_TAIL  */
    PEBL_LISTITEM_TAIL = 319,      /* PEBL_LISTITEM_TAIL  */
    PEBL_LOOP_TAIL1 = 320,         /* PEBL_LOOP_TAIL1  */
    PEBL_LOOP_TAIL2 = 321,         /* PEBL_LOOP_TAIL2  */
    PEBL_LT_TAIL = 322,            /* PEBL_LT_TAIL  */
    PEBL_MULTIPLY_TAIL = 323,      /* PEBL_MULTIPLY_TAIL  */
    PEBL_NE_TAIL = 324,            /* PEBL_NE_TAIL  */
    PEBL_NOT_TAIL = 325,           /* PEBL_NOT_TAIL  */
    PEBL_OR_TAIL = 326,            /* PEBL_OR_TAIL  */
    PEBL_POWER_TAIL = 327,         /* PEBL_POWER_TAIL  */
    PEBL_RETURN_TAIL = 328,        /* PEBL_RETURN_TAIL  */
    PEBL_SUBTRACT_TAIL = 329,      /* PEBL_SUBTRACT_TAIL  */
    PEBL_STATEMENTS_TAIL1 = 330,   /* PEBL_STATEMENTS_TAIL1  */
    PEBL_STATEMENTS_TAIL2 = 331,   /* PEBL_STATEMENTS_TAIL2  */
    PEBL_WHILE_TAIL = 332,         /* PEBL_WHILE_TAIL  */
    PEBL_WHILE_TAIL2 = 333,        /* PEBL_WHILE_TAIL2  */
    PEBL_FUNCTION_TAIL1 = 334,     /* PEBL_FUNCTION_TAIL1  */
    PEBL_FUNCTION_TAIL2 = 335,     /* PEBL_FUNCTION_TAIL2  */
    PEBL_FUNCTION_TAIL_LIBFUNCTION = 336, /* PEBL_FUNCTION_TAIL_LIBFUNCTION  */
    PEBL_FLOAT = 337,              /* PEBL_FLOAT  */
    PEBL_INTEGER = 338,            /* PEBL_INTEGER  */
    PEBL_STRING = 339,             /* PEBL_STRING  */
    PEBL_SYMBOL = 340,             /* PEBL_SYMBOL  */
    PEBL_LOCALVAR = 341,           /* PEBL_LOCALVAR  */
    PEBL_GLOBALVAR = 342,          /* PEBL_GLOBALVAR  */
    PEBL_FUNCTIONNAME = 343,       /* PEBL_FUNCTIONNAME  */
    PEBL_UMINUS = 344              /* PEBL_UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 39 "src/base/grammar.y"

  
  pInt    iValue;  /* For the lexical analyser. NUMBER tokens */
  pDouble fValue;
  char        *strValue; 
  PNode       *exp;    /* For expressions. */
  char        *symbol; /* The name of a variable*/
  char        *function; 

#line 158 "src/base/grammar.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_BASE_GRAMMAR_TAB_HPP_INCLUDED  */
