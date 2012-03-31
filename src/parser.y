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
  long intNumber;
  double floatNumber;
}

%token T_import T_from T_as T_if T_while T_for T_print T_class T_else
%token T_eq T_ne T_le T_ge T_and T_or

%token <Identifier> T_Identifier
%token <count> T_break T_return T_continue
%token <string> T_StringConst
%token <intNumber> T_IntConst
%token <floatNumber> T_FloatConst

%type <string_list> ModifierList AccessList
%type <Identifier> Identifier
%type <node> Function Variable Decl Expr Call Stmt IfStmt ReturnStmt
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
		|	ReturnStmt
		;


IfStmt		:	T_if '(' Expr ')' Stmt 		{ $$ = new IfStmt($3, $5, NULL); }
		|	T_if '(' Expr ')' Stmt T_else Stmt { $$ = new IfStmt($3, $5, $7); }
		;

WhileStmt	:	T_while	'(' Expr ')' Stmt 	{}
		;

ForStmt		:	T_for '(' Expr ')' Stmt		{}
		;

ReturnStmt	:	T_return Expr ';'		{ $$ = new ReturnStmt($2); }
		|	T_return ';'			{ $$ = new ReturnStmt(NULL); }

Function	:	'{' '}'				{ $$ = new Function(NULL, NULL); }
		|	'{' Stmts '}'			{ $$ = new Function(NULL, $2); }
		|	'{' '|' ParamList '|' Stmts '}'	{ $$ = new Function($3, $5); }
		;

Stmts           :       Stmts Stmt                      { ($$=$1)->push_back($2); }
                |       Stmt                            { ($$ = new list<Node*>)->push_back($1); }
                ;


ParamList	:	ParamList ',' Expr		{ ($$=$1)->push_back($3); }
		|	Expr				{ ($$ = new list<Node*>)->push_back($1); }
		|					{ $$ = new list<Node*>; }
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
		|	T_IntConst			{ $$ = new IntConst($1); }
		|	T_FloatConst			{ $$ = new FloatConst($1); }
		|	LValue
		|	'(' Expr ')'			{$$=$2;}
		|	Expr '+' Expr			{ $$ = new MathEquation(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), MathEquation::add); }
		|	Expr '-' Expr			{ $$ = new MathEquation(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), MathEquation::subtract); }
		|	Expr '*' Expr			{ $$ = new MathEquation(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), MathEquation::multiply); }
		| 	Expr '/' Expr			{ $$ = new MathEquation(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), MathEquation::devide); }
		|	Expr T_eq Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Equal); }
		|	Expr T_ne Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Not_Equal); }
		|	Expr T_le Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Less_Equal); }
		|	Expr '<' Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Less); }
		|	Expr T_ge Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Greater_Equal); }
		|	Expr '>' Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Greater); }
		|	Expr T_and Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::And); }
		|	Expr T_or Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Or); }
		;

LValue		:	Identifier			{}
		|	Expr '.' Identifier		{}
		|	Expr '[' Expr ']'		{}
		;

Identifier	:	T_Identifier			{ }
		;
%%
