%{
// only here for the union
#include <string>
#include <list>
#include "parserTree.h"

#include "parser.tab.hh"
#include "string.h"

//#include "parser.h"

#include <iostream>
using namespace std;

#include <stdlib.h>

%}

%option reentrant
%option bison-bridge
%option bison-locations
%option debug

%x COMMENT

IDENTIFIER	([a-zA-Z_][a-zA-Z0-9_]*)
OPERATOR	([-+/*%=.,;!<>()[\]{}])

%%

<*>" "			{;}
<*>\t			{;}
<*>\r			{;}
<*>\n			{;}

"//"[^\n]* 		{;}
"/*"			{BEGIN(COMMENT);}
<COMMENT>"*/"		{BEGIN(0);}
<COMMENT>.		{;}


"import"		{ return T_import; }
"from"			{ return T_from; }
"as"			{ return T_as; }
"if"			{ return T_if; }
"while"			{ return T_while; }
"for"			{ return T_for; }
"Print"			{ return T_print; }
"class"			{ return T_class; }

"break"			{ yylval->count=1; return T_break; }
"return"		{ yylval->count=1; return T_return; }
"continue"		{ yylval->count=1; return T_continue; }
"break"[0-9]+		{ yylval->count = atoi(yytext+5); return T_break; }
"return"[0-9]+		{ yylval->count = atoi(yytext+6); return T_return; }
"continue"[0-9]+	{ yylval->count = atoi(yytext+8); return T_continue; }
"=="			{ return T_eq; }
"!="			{ return T_ne; }
"<="			{ return T_le; }
">="			{ return T_ge; }
"&&"			{ return T_and; }
"||"			{ return T_or; }

{OPERATOR}		{ return yytext[0]; }



{IDENTIFIER}		{ strcpy(yylval->Identifier, yytext); return T_Identifier; }

\"[^\n]*\"		{ int len = strlen(yytext); yylval->string = new char[(len-1)]; strncpy(yylval->string, yytext+1, len-2); yylval->string[len-2]='\0'; return T_StringConst; }

%%



