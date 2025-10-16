/*////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/Pebl.l
//    Purpose:    Defines lexer for use with Flex
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//
//   
//
//     This file is part of the PEBL project.
//
//    PEBL is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    PEBL is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with PEBL; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///////////////////////////////////////////////////////////////////////////////

The following source defines a lexer for use with the GNU tool flex.
It identifies lexemes in a PEBL program.

*/
%option yylineno
%option noinput
%option nounput 
%{
#include <stdlib.h>
#include "PNode.h"            //This needed because PNode isn't defined in grammar.tab.hpp
#include "grammar.tab.hpp"
#include "../utility/PEBLUtility.h"
#include <string.h>
#include <math.h>


void yyerror(char *);

%}
bom        \xef\xbb\xbf
whitespace [ \t]+
comment     #.*
integer    [0-9]+
float      [0-9]*\.[0-9]+ 
if         [Ii][Ff]
loop       [Ll][Oo][Oo][Pp]
return     [Rr][Ee][Tt][Uu][Rr][Nn]
else       [Ee][Ll][Ss][Ee]
elseif     [Ee][Ll][Ss][Ee][Ii][Ff]
while      [Ww][Hh][Ii][Ll][Ee]
define     [Dd][Ee][Ff][Ii][Nn][Ee]
and        [Aa][Nn][Dd]
or         [Oo][Rr]
not        [Nn][Oo][Tt]
break      [Bb][Rr][Ee][Aa][Kk]
globalvar  g[A-Za-z0-9\_]*(\.[A-Za-z0-9\_]+)?
localvar   [a-fh-z][A-Za-z0-9\_]*(\.[A-Za-z0-9\_]+)?
functionname \:?[A-Z][A-Za-z0-9\_]*
string     \"[^\"]*[\"\n]
datum      [integer float string variable]
localprop {localvar}"."{property}
bomF      \xff\xfe     


%%
{bom}        ;       /* ignore byte-order-marks*/
{whitespace} ;       /* ignore whitespace*/
{comment}    ;       /* ignore comments  */
{integer}      { yylval.iValue = strtol(yytext,0,10);
                 return PEBL_INTEGER;
               };

{float}        { yylval.fValue = PEBLUtility::StringToPDouble(yytext);
                 return PEBL_FLOAT;
               };

\+        return  PEBL_ADD;
\<\-      return  PEBL_ASSIGN;
\,        return  PEBL_COMMA;
\/        return  PEBL_DIVIDE;
\^        return  PEBL_POWER;
\.        return  PEBL_DOT;
\=\=      return  PEBL_EQ;
\>\=      return  PEBL_GE;
\:        return PEBL_COLON;
\>        return  PEBL_GT;
\(        return  PEBL_LPAREN;
\{        return  PEBL_LBRACE;
\[        return  PEBL_LBRACKET;
\<\=      return  PEBL_LE;
\<        return  PEBL_LT;
\*        return  PEBL_MULTIPLY;
\<\>      return  PEBL_NE;
\!\=      return  PEBL_NE;
\~\=      return  PEBL_NE;
\}        return  PEBL_RBRACE;
\]        return  PEBL_RBRACKET;
\)        return  PEBL_RPAREN;
\-        return  PEBL_SUBTRACT;
\;        return  PEBL_NEWLINE;
{if}            return  PEBL_IF;
{loop}          return  PEBL_LOOP;
{return}        return  PEBL_RETURN;
{else}          return  PEBL_ELSE;
{elseif}        return  PEBL_ELSEIF;
{while}         return  PEBL_WHILE;
{define}        return  PEBL_DEFINE;
{and}           return PEBL_AND;
{or}            return PEBL_OR;
{not}           return  PEBL_NOT;
{break}         return PEBL_BREAK;

{localvar} {   /*Make a deep copy of the variable*/
               yylval.symbol = strdup(yytext);
               return PEBL_LOCALVAR;
           };

{globalvar} {   /*Make a deep copy of the variable*/
               yylval.symbol = strdup(yytext);
               return PEBL_GLOBALVAR;
           };

{functionname} { /*Make a deep copy of the function name*/
                yylval.function =strdup(yytext);
                return PEBL_FUNCTIONNAME;
};

{string} {          /*Make a deep copy of the string*/
  yylval.strValue = strdup(yytext+1); /*skip open quote*/

if(yylval.strValue[yyleng-2] != '"')     /*"*/
	    { char message[80];
	    snprintf(message, sizeof(message), "Unterminated character string in line: %d\n",yylineno  );
	  /*  yyerror(message);*/
	    }
  else
    yylval.strValue[yyleng-2] = '\0'; /*remove close quote*/
  return PEBL_STRING;
};

\r\n      {return PEBL_NEWLINE;}
\n        {return PEBL_NEWLINE;}
\r        {return PEBL_NEWLINE;}
{bomF}     {
            char message[151];
            snprintf(message,150,"\n  File appears to be in UTF-16 Unicode format.\n  PEBL can only read UTF-8 or ASCII formats. Resave file as UTF-8 or ASCII and try again.");
            yyerror(message);
           }

.          { char message[100];
                 snprintf(message,99,"\nUnknown characters in line %d: [%s]\n",yylineno,yytext);
                 yyerror(message);
           }

<<EOF>>    {
                 /* Ensure file ends with a newline token for proper parsing.
                  * This handles files that don't end with a trailing newline. */
                 static int eof_newline_sent = 0;
                 if (!eof_newline_sent) {
                     eof_newline_sent = 1;
                     return PEBL_NEWLINE;
                 }
                 eof_newline_sent = 0;  /* Reset for next file */
                 yyterminate();
           }
%%
int yywrap(void) {
    return 1;
}
