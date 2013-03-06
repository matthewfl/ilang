%{


#include "ilang.h"
#include "parser.h"
#include "parserTree.h"
#include "parser.tab.hh"
#include "import.h"
#include "debug.h"

//using namespace ilang; // adding this in cause some conflicts with names
using namespace ilang::parserNode;
#include <iostream>
#include <string>
#include <list>
#include <map>
using namespace std;

struct yyltype;
struct yyltype yylloc;

void yyerror(YYLTYPE *loc, void *, ilang::parser_data*, const char *msg) {
  using namespace std;
  cerr << "error: " << msg << endl << yylloc.first_line;

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
  std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *object_map;
  std::pair<ilang::parserNode::Variable*, ilang::parserNode::Node*> *object_pair; // needs to be a pointer even though it is quick in use
  ilang::parserNode::Node *node;
  long intNumber;
  double floatNumber;
}

%token T_import T_from T_as T_if T_while T_for T_print T_class T_else T_object T_new T_assert T_go
%token T_eq T_ne T_le T_ge T_and T_or
%token T_plusEqual T_subEqual T_mulEqual T_divEqual

%right T_else
%left ';'
%left '='
%left T_plusEqual T_subEqual T_mulEqual T_divEqual
%left T_and T_or
%left T_eq T_ne T_le '<' '>'
%left '+' '-'
%left '*' '/'
%left '!'
%right uMinus
%left '.' '['

%token <Identifier> T_Identifier
%token <count> T_break T_return T_continue
%token <string> T_StringConst
%token <intNumber> T_IntConst
%token <floatNumber> T_FloatConst

%type <string_list> ModifierList AccessList ImportLoc
%type <Identifier> Identifier
%type <node> Function Variable LValue Decl Expr Call Stmt IfStmt ReturnStmt Object Class Array WhileStmt ForStmt Args
%type <node_list> Stmts ParamList DeclList ExprList ArgsList ProgramList
%type <object_map> ObjectList
%type <object_pair> ObjectNode


%%
Program		:	Imports ProgramList		{ parser_handle->head = new ilang::parserNode::Head($2, parser_handle->import); }
		;

Imports		:	Imports Import			{ }
		|					{ }
		;

Import		:	T_import ImportLoc			{ assert(parser_handle->import); parser_handle->import->push(NULL, $2); }
		|	T_from	ImportLoc T_import ImportLoc	{ assert(parser_handle->import); parser_handle->import->push($2, $4); }
		;

ImportLoc	:	ImportLoc '.' T_Identifier 	{ ($$=$1)->push_back($3); }
		|	T_Identifier			{ ($$=new std::list<std::string>)->push_back($1); }
		;

DeclList	:	DeclList Decl 			{ ($$=$1)->push_back($2); }
		|	Decl				{ ($$ = new list<Node*>)->push_back($1); }
		;

Decl		:	Variable '=' Expr ';'		{ $$ = new AssignExpr(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3)); }
		;

ProgramList	:	ProgramList Expr ';'		{ ($$=$1)->push_back($2); }
		|	Expr ';'			{ ($$=new list<Node*>)->push_back($1); }
		;

Variable	:	LValue				{ }
		|	ModifierList AccessList		{ $$ = new Variable($2, $1); }
		;

LValue		:	Identifier			{ $$ = new FieldAccess(NULL, $1); }
		|	Expr '.' Identifier		{ $$ = new FieldAccess($1, $3); }
		|	Expr '[' Expr ']'		{ $$ = new ArrayAccess($1, $3); }
		;

ModifierList	:	ModifierList Identifier		{ ($$ = $1)->push_back($2); }
		|	Identifier			{ ($$ = new list<string>)->push_back($1); }
		;

AccessList	:	AccessList '.' Identifier	{ ($$ = $1)->push_back($3); }
		|	Identifier			{ ($$ = new list<string>)->push_back($1);}
		;

Stmt		:	';'				{ $$ = new Function(NULL, NULL) ; /* not quite right, but should work for the most part */}
		| 	Expr ';'
		|	Function
		|	IfStmt
		|	WhileStmt
		|	ForStmt
		|	ReturnStmt
		|	Decl
		;

ObjectNode	:	Variable ':' Expr		{ $$ = new std::pair<ilang::parserNode::Variable*, ilang::parserNode::Node*>(dynamic_cast<ilang::parserNode::Variable*>($1), $3); /* should not have any problems with the cast */  }
		|	Variable			{ $$ = new std::pair<ilang::parserNode::Variable*, ilang::parserNode::Node*>(dynamic_cast<ilang::parserNode::Variable*>($1), NULL); }
		;

ObjectList	:	ObjectList ',' ObjectNode	{ ($$=$1)->insert(*$3); delete $3; }
		|	ObjectNode			{ $$ = new std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*>; $$->insert(*$1); delete $1;  }
		;

Object		:	T_object '{' ObjectList '}'	{ $$ = new Object($3); }
		|	T_object '{' '}'		{ $$ = new Object(new std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*>); }
		;

Class		:	T_class '{' ObjectList '}'	{ $$ = new Class(new std::list<Node*>, $3); }
		|	T_class	'(' ParamList ')' '{' ObjectList '}'	{ $$ = new Class($3, $6); }
		;

Array		:	'[' ModifierList '|' ParamList ']' { $$ = new Array($4, $2); }
		| 	'[' ParamList ']'		   { $$ = new Array($2, NULL); }
		;

IfStmt		:	T_if '(' Expr ')' Stmt 		{ $$ = new IfStmt($3, $5, NULL); }
		|	T_if '(' Expr ')' Stmt T_else Stmt { $$ = new IfStmt($3, $5, $7); }
		;

WhileStmt	:	T_while	'(' Expr ')' Stmt 	{ $$ = new WhileStmt ($3, $5); }
		;

ForStmt		:	T_for '(' Expr ';' Expr ';' Expr ')' Stmt	{ $$ = new ForStmt($3, $5, $7, $9); }
		;

ReturnStmt	:	T_return Expr ';'		{ $$ = new ReturnStmt($2); }
		|	T_return ';'			{ $$ = new ReturnStmt(NULL); }

Function	:	'{' '}'				{ $$ = new Function(NULL, NULL); }
		|	'{' Stmts '}'			{ $$ = new Function(NULL, $2); }
		|	'{' '|' ArgsList '|' Stmts '}'	{ $$ = new Function($3, $5); }
		|	'{' '|' ArgsList '|' '}'	{ $$ = new Function($3, NULL); }
		;

ArgsList	:	Args				{ ($$ = new std::list<Node*>)->push_back($1); }
		|	ArgsList ',' Args		{ ($$=$1)->push_back($3); }
		;

Args		:	ModifierList Identifier		{ list<string> *n = new list<string>; n->push_back($2); $$ = new Variable(n, $1); }
		|	Identifier			{ $$ = new FieldAccess(NULL, $1); }

Stmts           :       Stmts Stmt                      { ($$=$1)->push_back($2); }
                |       Stmt                            { ($$ = new list<Node*>)->push_back($1); }
                ;


ParamList	:	ParamList ',' Expr		{ ($$=$1)->push_back($3); }
		|	Expr				{ ($$ = new list<Node*>)->push_back($1); }
		|					{ $$ = new list<Node*>; }
		;

Call		:	Expr '(' ParamList ')'		{ $$ = new Call(dynamic_cast<Value*>($1), $3); }
		|	T_print '(' ParamList ')'	{ $$ = new PrintCall($3); }
		|	T_new '(' ParamList ')'		{ $$ = new NewCall($3); }
		|	T_assert '(' ParamList ')'	{ $$ = new AssertCall(@1.first_line, parser_handle->fileName, $3); }
		|	T_import '(' ParamList ')'	{ $$ = new ImportCall($3); }
		|	T_go '(' ParamList ')'		{ $$ = new ThreadGoCall($3); }	
		;

ExprList	:	ExprList Expr			{ ($$=$1)->push_back($2); }
		|	Expr				{ ($$ = new list<Node*>)->push_back($1); }
		;

Expr		:	Function			{}
		|	Class
		|	Object
		|	Array
		|	Call
		|	Variable
		|	Variable '=' Expr		{ $$ = new AssignExpr(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3)); }
		|	T_StringConst			{ $$ = new StringConst($1); }
		|	T_IntConst			{ $$ = new IntConst($1); }
		|	T_FloatConst			{ $$ = new FloatConst($1); }
		|	'(' Expr ')'			{$$=$2;}
		|	Expr '+' Expr			{ $$ = new MathEquation(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), MathEquation::add); }
		|	Expr '-' Expr			{ $$ = new MathEquation(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), MathEquation::subtract); }
		|	'-' Expr %prec uMinus		{ $$ = new MathEquation(dynamic_cast<Value*>($2), NULL, MathEquation::uMinus); }
		|	Expr '*' Expr			{ $$ = new MathEquation(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), MathEquation::multiply); }
		| 	Expr '/' Expr			{ $$ = new MathEquation(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), MathEquation::divide); }
		|	Expr T_eq Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Equal); }
		|	Expr T_ne Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Not_Equal); }
		|	Expr T_le Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Less_Equal); }
		|	Expr '<' Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Less); }
		|	Expr T_ge Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Greater_Equal); }
		|	Expr '>' Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Greater); }
		|	Expr T_and Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::And); }
		|	Expr T_or Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($1), dynamic_cast<Value*>($3), LogicExpression::Or); }
		|	'!' Expr			{ $$ = new LogicExpression(dynamic_cast<Value*>($2), NULL, LogicExpression::Not); }
		|	Variable T_plusEqual Expr	{ $$ = new SingleExpression(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3), SingleExpression::add); }
		|	Variable T_subEqual Expr	{ $$ = new SingleExpression(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3), SingleExpression::subtract); }
		|	Variable T_mulEqual Expr	{ $$ = new SingleExpression(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3), SingleExpression::multiply); }
		|	Variable T_divEqual Expr	{ $$ = new SingleExpression(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3), SingleExpression::divide); }
		;

Identifier	:	T_Identifier			{ }
		;
%%
