%{


#include "ilang.h"
#include "parser.h"
#include "parserTree.h"
#include "parser.tab.hh"
#include "import.h"
#include "debug.h"
#include "identifier.h"

//using namespace ilang; // adding this in cause some conflicts with names
using namespace ilang::parserNode;
#include <iostream>
#include <string>
#include <list>
#include <map>
using namespace std;
using ilang::Identifier;

//struct yyltype;
//struct yyltype yylloc;


void yyerror(YYLTYPE *loc, void *, ilang::parser_data *parser_handle, const char *msg) {
  using namespace std;
  //cerr << "error: " << msg << endl << yylloc.first_line;
  parser_handle->error_count++;
  cerr << "error @" << loc->first_line << ": " << msg << "\n";
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
%glr-parser


%union {
  //char Identifier[128];
  unsigned long identifier;
  int count;
  char *string;
  std::vector<ilang::Identifier> *identifier_list;
  std::list<std::string> *string_list;
  std::list<ilang::parserNode::Node*> *node_list;
  ilang::parserNode::Node *node;
  long intNumber;
  double floatNumber;
}

%token T_import T_from T_as T_if T_while T_for T_print T_class T_else T_object T_new T_assert T_go T_local T_dynamic
%token T_eq T_ne T_le T_ge T_and T_or
%token T_plusEqual T_subEqual T_mulEqual T_divEqual

%right LowerThanElse
%right T_else
%left ';'
%right ModListPrec
%right LowerThanEqual
%left '='
%left T_plusEqual T_subEqual T_mulEqual T_divEqual
%left T_and T_or
%nonassoc T_eq T_ne T_le T_ge '<' '>'
%left '+' '-'
%left '*' '/'
%left '!'
%right uMinus
%left TuplePrec
%left '(' ')' PrensPrec
%left '.' '['

%token <identifier> T_Identifier
%token <count> T_break T_return T_continue
%token <string> T_StringConst
%token <intNumber> T_IntConst
%token <floatNumber> T_FloatConst

%type <identifier_list> ImportLoc
%type <identifier> Identifier
%type <node> Function FunctionBasic Variable LValue Expr Expr_ ExprType Call Stmt IfStmt ReturnStmt Object Class Array WhileStmt ForStmt Args ModifierType TupleRHS TupleRHScnt TupleLHS TupleLHScnt
%type <node_list> Stmts ParamList ArgsList ProgramList ModifierList TupleLHSinner TupleLHSinner2 TupleRHSinner TupleRHSinner2


%%
Program		:	Imports ProgramList		{ parser_handle->head = new ilang::parserNode::Head($2, parser_handle->import); }
		;

Imports		:	Imports Import			{ }
		|	%empty				{ }
		;

Import		:	T_import ImportLoc			{ assert(parser_handle->import); parser_handle->import->push(NULL, $2); }
		|	T_from	ImportLoc T_import ImportLoc	{ assert(parser_handle->import); parser_handle->import->push($2, $4); }
		;

ImportLoc	:	ImportLoc '.' T_Identifier 	{ ($$=$1)->push_back(Identifier($3)); }
		|	T_Identifier			{ ($$=new std::vector<ilang::Identifier>)->push_back(Identifier($1)); }
		;

ProgramList	:	ProgramList Expr ';'		{ ($$=$1)->push_back($2); }
		|	Expr ';'			{ ($$=new list<Node*>)->push_back($1); }
		;

Variable	:	ModifierList Identifier		{ $$ = new Variable(Identifier($2), $1); }
		|	LValue
		|	TupleLHS
		;

LValue		:	Identifier			{ $$ = new FieldAccess(NULL, Identifier($1)); }
		|	ExprType '.' Identifier		{ $$ = new FieldAccess($1, Identifier($3)); }
		|	ExprType '[' Expr ']'		{ $$ = new ArrayAccess($1, $3); }
		;

ModifierList	:	ModifierList ModifierType %prec ModListPrec	{ ($$ = $1)->push_back($2); }
		|	ModifierType %prec ModListPrec			{ ($$ = new list<ilang::parserNode::Node*>)->push_back($1); }
		;

ModifierType	:	ExprType %prec ModListPrec
		|	T_local				{ $$ = new VariableScopeModifier(ilang::VariableType::t_local); }
		|	T_dynamic			{ $$ = new VariableScopeModifier(ilang::VariableType::t_dynamic); }
		;

Stmt		: 	Expr ';'
		|	Function
		|	IfStmt
		|	WhileStmt
		|	ForStmt
		|	ReturnStmt
		;

Object		:	T_object FunctionBasic			{ $$ = new Object(dynamic_cast<Function*>($2)); }
		;

Class		:	T_class FunctionBasic			{ $$ = new Class(new std::list<Node*>, dynamic_cast<Function*>($2)); }
		|	T_class	'(' ParamList OptComma ')' FunctionBasic	{ $$ = new Class($3, dynamic_cast<Function*>($6)); }
		;

Array		:	'[' ModifierList '|' ParamList OptComma ']'	{ $$ = new Array($4, $2); }
		| 	'[' ParamList OptComma ']'				{ $$ = new Array($2, NULL); }
		;

IfStmt		:	T_if '(' Expr ')' Stmt %prec LowerThanElse	{ $$ = new IfStmt($3, $5, NULL); }
		|	T_if '(' Expr ')' Stmt T_else Stmt 		{ $$ = new IfStmt($3, $5, $7); }
		;

WhileStmt	:	T_while	'(' Expr ')' Stmt 			{ $$ = new WhileStmt ($3, $5); }
		;

ForStmt		:	T_for '(' Expr ';' Expr ';' Expr ')' Stmt	{ $$ = new ForStmt($3, $5, $7, $9); }
		;

ReturnStmt	:	T_return Expr ';'		{ $$ = new ReturnStmt($2); }
		|	T_return ';'			{ $$ = new ReturnStmt(NULL); }
		;

FunctionBasic	:	'{' '}'				{ $$ = new Function(NULL, NULL); }
		|	'{' Stmts '}'			{ $$ = new Function(NULL, $2); }
		;

Function	:	FunctionBasic
		|	'{' '|' ArgsList OptComma '|' Stmts '}'	{ $$ = new Function($3, $6); }
		|	'{' '|' ArgsList OptComma '|' '}'	{ $$ = new Function($3, NULL); }
		;

ArgsList	:	Args				{ ($$ = new std::list<Node*>)->push_back($1); }
		|	ArgsList ',' Args		{ ($$=$1)->push_back($3); }
		;

Args		:	ModifierList Identifier		{ $$ = new Variable(Identifier($2), $1); }
		|	Identifier			{ $$ = new Variable(Identifier($1), NULL); }
		;

Stmts           :       Stmts Stmt                      { ($$=$1)->push_back($2); }
                |       Stmt                            { ($$ = new list<Node*>)->push_back($1); }
		|	error				{ $$ = new list<Node*>; }
		;


ParamList	:	ParamList ',' Expr		{ ($$=$1)->push_back($3); }
		|	Expr				{ ($$ = new list<Node*>)->push_back($1); }
		|					{ $$ = new list<Node*>; }
		;

Call		:	ExprType '(' ParamList OptComma')'	{ $$ = new Call(dynamic_cast<Value*>($1), $3); }
		|	T_print '(' ParamList OptComma')'	{ $$ = new PrintCall($3); }
		|	T_assert '(' ParamList OptComma')' 	{ $$ = new AssertCall(@1.first_line, parser_handle->fileName, $3); }
		|	T_import '(' ParamList OptComma')'	{ $$ = new ImportCall($3); }
		|	T_go '(' ParamList OptComma')'		{ $$ = new ThreadGoCall($3); }
		;

TupleRHS	:	'(' TupleRHScnt ',' TupleRHSinner OptComma ')'		{ $4->push_front($2); $$ = new TupleRHS($4); }
		;

TupleRHSinner	:	TupleRHSinner2 ',' TupleRHScnt		{ ($$=$1)->push_back($3); }
		|	TupleRHScnt				{ ($$ = new list<Node*>)->push_back($1); }
		|						{ $$ = new list<Node*>; }
		;

TupleRHSinner2	:	TupleRHSinner2 ',' TupleRHScnt		{ ($$=$1)->push_back($3); }
		|	TupleRHScnt				{ ($$ = new list<Node*>)->push_back($1); }
		;

TupleRHScnt	:	Identifier '=' Expr %dprec 2		 { $$ = new NamedValue(Identifier($1), dynamic_cast<Value*>($3)); }
		|	Expr %dprec 1
		;

TupleLHS	:	'(' TupleLHScnt ',' TupleLHSinner OptComma ')'		{ $4->push_front($2); $$ = new TupleLHS($4); }
		;

TupleLHSinner	:	TupleLHSinner2 ',' TupleLHScnt		{ ($$=$1)->push_back($3); }
		|	TupleLHScnt				{ ($$ = new list<Node*>)->push_back($1); }
		|						{ $$ = new list<Node*>; }
		;

TupleLHSinner2	:	TupleLHSinner2 ',' TupleLHScnt		{ ($$=$1)->push_back($3); }
		|	TupleLHScnt				{ ($$ = new list<Node*>)->push_back($1); }
		;

TupleLHScnt	:	Args '=' Expr			{ $$ = new AssignExpr(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3)); }
		|	Args
		;

Expr		:	Expr_
		|	error				{ $$ = new IntConst(0); }
		;

Expr_		:	ExprType
		|	Variable '=' Expr %prec LowerThanEqual	{ $$ = new AssignExpr(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3)); }
		|	T_StringConst			{ $$ = new StringConst($1); }
		|	T_IntConst			{ $$ = new IntConst($1); }
		|	T_FloatConst			{ $$ = new FloatConst($1); }
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
		|	LValue T_plusEqual Expr	{ $$ = new SingleExpression(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3), SingleExpression::add); }
		|	LValue T_subEqual Expr	{ $$ = new SingleExpression(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3), SingleExpression::subtract); }
		|	LValue T_mulEqual Expr	{ $$ = new SingleExpression(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3), SingleExpression::multiply); }
		|	LValue T_divEqual Expr	{ $$ = new SingleExpression(dynamic_cast<Variable*>($1), dynamic_cast<Value*>($3), SingleExpression::divide); }
		|	TupleRHS %prec TuplePrec
		;

/* ExprType is something that can be used for the type checking */
ExprType	:	Function
		|	Class
		|	Object
		|	Array
		|	Call
		|	'(' Expr ')'	%prec PrensPrec		{ $$ = $2; }
		|	LValue
		;

Identifier	:	T_Identifier			{ }
		;

OptComma	:	','
		|	%empty
		;

%%
