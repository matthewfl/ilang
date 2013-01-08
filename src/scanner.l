%{
// only here for the union
#include <string>
#include <list>
#include "parserTree.h"
#include "parser.h"

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
%option noyywrap

%x COMMENT

IDENTIFIER	([a-zA-Z_][a-zA-Z0-9_]*)
OPERATOR	([-+/*%=.,;:!<>()[\]{}\|])

%%

<*>" "			{;}
<*>\t			{;}
<*>\r			{;}
<*>\n			{yylloc->first_line++;}

"//"[^\n]* 		{;}
"/*"			{BEGIN(COMMENT);}
<COMMENT>"*/"		{BEGIN(0);}
<COMMENT>.		{;}


"import"		{ return T_import; }
"from"			{ return T_from; }
"as"			{ return T_as; }
"if"			{ return T_if; }
"else"			{ return T_else; }
"while"			{ return T_while; }
"for"			{ return T_for; }
"Print"			{ return T_print; }
"class"			{ return T_class; }
"object"		{ return T_object; }
"new"			{ return T_new; }
"assert"		{ return T_assert; }
"go"			{ return T_go; }

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

"+="			{ return T_plusEqual; }
"-="			{ return T_subEqual; }
"*="			{ return T_mulEqual; }
"/="			{ return T_divEqual; }

{OPERATOR}		{ return yytext[0]; }



{IDENTIFIER}		{ strcpy(yylval->Identifier, yytext); return T_Identifier; }

\"([^"\n]|\\\")*?\"		{ int len = strlen(yytext); yylval->string = new char[(len-1)]; strncpy(yylval->string, yytext+1, len-2); yylval->string[len-2]='\0'; return T_StringConst; }
[0-9]+			{ yylval->intNumber = atol(yytext); return T_IntConst; }
[0-9]*"."[0-9]*([Ee][-+]?[0-9]+)?	{ yylval->floatNumber = atof(yytext); return T_FloatConst; }

%%

