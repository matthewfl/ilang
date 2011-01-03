%{


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

"break"			{ yylval->count=1; return T_break; }
"return"		{ yylval->count=1; return T_return; }
"continue"		{ yylval->count=1; return T_continue; }
"break"[0-9]+		{ yylval->count = atoi(yytext+5); return T_break; }
"return"[0-9]+		{ yylval->count = atoi(yytext+6); return T_return; }
"continue"[0-9]+	{ yylval->count = atoi(yytext+8); return T_continue; }

{OPERATOR}		{ return yytext[0]; }



{IDENTIFIER}		{ strcpy(yylval->Identifier, yytext); return T_Identifier; }



%%



