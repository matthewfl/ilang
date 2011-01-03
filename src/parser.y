%{


#include "ilang.h"
#include "parser.h"
#include "parserTree.h"
#include "parser.tab.hh"

using namespace ilang;
#include <iostream>
using namespace std;

void yyerror(YYLTYPE *loc, void *, ilang::parser_data*, const char *msg) {
  using namespace std;
  cout << "error: " << msg << endl;
}

#define YYDEBUG 1

%}

%define api.pure
%defines
%locations
%parse-param {void * yyscanner}
%lex-param   {void * yyscanner}
%parse-param {ilang::parser_data * parser_handle}
%debug
%error-verbose

%union {
  char Identifier[40];
  int count;
}

%token T_import T_from T_as T_if T_while T_for


%token <Identifier> T_Identifier
%token <count> T_break T_return T_continue


%%

Program		:	Imports DeclList		{ cout << "head"; /*parser_handle.head = new ilang::parserNode::Head($2); */}
		;

Imports		:	Imports Import			{ cout << "import\n"; }
		|					{ cout << "empty import\n"; }
		;

Import		:	T_import ImportLoc			{}
		|	T_from	ImportLoc T_import ImportLoc	{}
		;	

ImportLoc	:	ImportLoc "." T_Identifier 	{}
		|	T_Identifier			{}
		;

DeclList	:	DeclList Decl 			{}
		|	Decl				{}
		;

Decl		:	Identifier '=' Function ';'	{}
		|	Identifier '=' Expr ';'		{}
		;

Stmt		:	';'
		| 	Expr ';'
		|	Function
		|	IfStmt
		|	WhileStmt
		|	ForStmt
		;

IfStmt:'a';
WhileStmt:'b';
ForStmt:'c';

Function	:	'{' ExprList '}'		{}
		|	'{' '|' PramList '|' ExprList '}'	{}
		;

PramList	:	PramList ',' Identifier		{}
		|	Identifier			{}
		;

ExprList	:	ExprList Expr			{}
		|	Expr				{}
		;	
	
Expr		:
		
		;

LValue		:	Identifier			{}
		|	Expr '.' Identifier		{}
		|	Expr '[' Expr ']'		{}

Identifier	:	T_Identifier			{cout << $1 << endl; }
		;

%%
