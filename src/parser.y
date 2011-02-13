%{


#include "ilang.h"
#include "parser.h"
#include "parserTree.h"
#include "parser.tab.hh"

using namespace ilang;
using namespace ilang::parserNode;
#include <iostream>
#include <string>
#include <list>
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
  std::list<std::string> *string_list;
  std::list<ilang::parserNode::Node*> *node_list;
  ilang::parserNode::Node *node;
}

%token T_import T_from T_as T_if T_while T_for


%token <Identifier> T_Identifier
%token <count> T_break T_return T_continue

%type <string_list> ModifierList
%type <Identifier> Identifier
%type <node> Function
%type <node_list> Stmts


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

Decl		:	Variable '=' Expr ';'		{}
		;

Variable	:	Identifier			{}
		;

ModifierList	:	ModifierList Identifier		{ ($$ = $1)->push_back($1); }
		|	Identifier			{($$ = new list<string>)->push_back($1); }
		|					{ $$ = new list<string>; }
		;

Stmt		:	';'
		| 	Expr ';'
		|	Function
		|	IfStmt
		|	WhileStmt
		|	ForStmt
		;


IfStmt		:	T_if '(' Expr ')' Stmt 		{}
		;

WhileStmt	:	T_while	'(' Expr ')' Stmt 	{}
		;

ForStmt		:	T_for '(' Expr ')' Stmt		{}
		;

Function	:	'{' Stmts '}'			{ $$ = new Function; }
		|	'{' '|' PramList '|' Stmts '}'	{ $$ = new Function; }
		;

Stmts           :       Stmts Stmt                      { ($$=$1); }
                |       Stmt                            { ($$ = new std::list<Node*>); }
                ;


PramList	:	PramList ',' Identifier		{}
		|	Identifier			{}
		;

Call		:	Identifier '(' PramList ')'	{}

ExprList	:	ExprList Expr			{}
		|	Expr				{}
		;	
	
Expr		:	Function			{}
		|	Call
		;

LValue		:	Identifier			{}
		|	Expr '.' Identifier		{}
		|	Expr '[' Expr ']'		{}
		;

Identifier	:	T_Identifier			{ }
		;

%%
