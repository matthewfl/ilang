%{


#include "ilang.h"
#include "parser.h"
#include "parserTree.h"
#include "parser.tab.hh"

//using namespace ilang; // adding this in cause some conflicts with names
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
  char *string;
  std::list<std::string> *string_list;
  std::list<ilang::parserNode::Node*> *node_list;
  ilang::parserNode::Node *node;
}

%token T_import T_from T_as T_if T_while T_for T_print


%token <Identifier> T_Identifier
%token <count> T_break T_return T_continue
%token <string> T_StringConst

%type <string_list> ModifierList AccessList
%type <Identifier> Identifier
%type <node> Function Variable Decl Expr Call Stmt
%type <node_list> Stmts ParamList DeclList ExprList


%%

Program		:	Imports DeclList		{ parser_handle->head = new ilang::parserNode::Head($2); }
		;

Imports		:	Imports Import			{ cout << "import\n"; }
		|					{ cout << "empty import\n"; }
		;

Import		:	T_import ImportLoc			{}
		|	T_from	ImportLoc T_import ImportLoc	{}
		;	

ImportLoc	:	ImportLoc '.' T_Identifier 	{}
		|	T_Identifier			{}
		;

DeclList	:	DeclList Decl 			{ ($$=$1)->push_back($2); }
		|	Decl				{ ($$ = new list<Node*>)->push_back($1); }
		;

Decl		:	Variable '=' Expr ';'		{ $$ = new AssignExpr(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3)); }
		;

Variable	:	AccessList			{ $$ = new Variable($1, new list<string>); }
		|	ModifierList AccessList		{ $$ = new Variable($2, $1); }
		;


ModifierList	:	ModifierList Identifier		{ ($$ = $1)->push_back($2); }
		|	Identifier			{ ($$ = new list<string>)->push_back($1); }
		;

AccessList	:	AccessList '.' Identifier	{ ($$ = $1)->push_back($3); }
		|	Identifier			{ ($$ = new list<string>)->push_back($1);}
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

Function	:	'{' '}'				{ $$ = new Function; }
		|	'{' Stmts '}'			{ $$ = new Function; }
		|	'{' '|' ParamList '|' Stmts '}'	{ $$ = new Function; }
		;

Stmts           :       Stmts Stmt                      { ($$=$1)->push_back($2); }
                |       Stmt                            { ($$ = new list<Node*>)->push_back($1); }
                ;


ParamList	:	ParamList ',' Expr		{ ($$=$1)->push_back($3); }
		|	Expr				{ ($$ = new list<Node*>)->push_back($1); }
		;

Call		:	Variable '(' ParamList ')'	{ $$ = new Call(dynamic_cast<Variable*>($1), $3); }
		|	T_print '(' ParamList ')'	{ $$ = new PrintCall($3); }

ExprList	:	ExprList Expr			{ ($$=$1)->push_back($2); }
		|	Expr				{ ($$ = new list<Node*>)->push_back($1); }
		;	
	
Expr		:	Function			{}
		|	Call
		|	Variable
		|	T_StringConst			{ $$ = new StringConst($1); }
		|	LValue
		;

LValue		:	Identifier			{}
		|	Expr '.' Identifier		{}
		|	Expr '[' Expr ']'		{}
		;

Identifier	:	T_Identifier			{ }
		;
%%
