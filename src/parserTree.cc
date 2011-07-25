#include "parserTree.h"
#include "parser.h"
#include "scope.h"
#include "debug.h"

#include <iostream>
using namespace std;

namespace ilang {
  namespace parserNode {
    Head::Head(list<Node*> *declars): Declars(declars) {
      debug( "head " << declars->size() );
      Run();
    }
    void Head::Run () {
      FileScope scope;
      for(list<Node*>::iterator it = Declars->begin(); it !=  Declars->end(); it++) {
	debug( "calling run" )
	(*it)->Run(&scope);
	scope.Debug();
      }
      list<ilang::Value*> v;
      boost::any_cast<Function*>(scope.lookup("main")->Get()->Get())->Call(&scope, v);
      //return RunReturn(new ilang::Value);
    }


    StringConst::StringConst(char *str) :string(str){}
    void StringConst::Run(Scope *scope) {
      //return RunReturn(new ilang::Value(string));
    }
    ValuePass StringConst::GetValue (Scope *scope) {
      debug( "string get value" );
      return ValuePass(new ilang::Value(std::string(string)));
    }
    void IfStmt::Run(Scope *scope) {}
    void WhileStmt::Run(Scope *scope) {}
    void ForStmt::Run(Scope *scope) {}



    IfStmt::IfStmt () {

    }
    ForStmt::ForStmt () {

    }
    WhileStmt::WhileStmt () {

    }

    Function::Function (list<Node*> *p, list<Node*> *b):body(b), params(p) {
      debug( "\t\t\tfunction constructed" );

    }
    void Function::Run(Scope *scope) {
      list<ilang::Value*> p;
      Call(scope, p);
    }
    ValuePass Function::GetValue(Scope *scope) {
      debug("Function get value");
      return ValuePass(new ilang::Value(this));
    }
    void Function::Call(list<ilang::Value*> params) {
      // this needs to be removed
      Call(NULL, params);
    }
    void Function::Call(Scope *_scope, list<ilang::Value*> p) {
      FunctionScope scope(_scope);
      debug("function called");
      if(params) { // the parser set params to NULL when there are non
	list<Node*>::iterator it = params->begin();
	for(ilang::Value * v : p) {
	  if(it==params->end()) break;
	  assert(dynamic_cast<Variable*>(*it));
	  dynamic_cast<parserNode::Variable*>(*it)->Set(&scope, ValuePass(v));
	  it++;
	}
      }
      for(Node *n : *body) {
	assert(n);
	debug("function run");
	n->Run(&scope);
      }
      
    }

    Variable::Variable (list<string> *n, list<string> *mod):
      name(n), modifiers(mod) {
      //cout << "\t\t\t" << name << "\n";
    }
    void Variable::Run (Scope *scope) {
      debug("\t\t\tSetting variable: " << name->front());
    }
    void Variable::Set (Scope *scope, ValuePass var) {
      scope->Debug();
      ilang::Variable *v;
      if(!modifiers->empty())
	v = scope->forceNew(name->front(), *modifiers);
      else
	v = scope->lookup(name->front());
      assert(v);
      v->Set(var);
      debug("Set: " << name->front() << " " << var << " " << v->Get());
      scope->Debug();
    }
    ilang::Variable * Variable::Get(Scope *scope) {
      scope->Debug();
      ilang::Variable *v;
      v = scope->lookup(name->front());
      assert(v);
      debug("Get: " << name->front() << " " << v->Get());
      return v;
    }
    Call::Call (Variable *call, list<Node*> *args):
      calling(call), params(args) {
      debug("\t\t\tCalling function");
    }
    void Call::Run(Scope *scope) {
      debug("call run");
      GetValue(scope);
    }
    ValuePass Call::GetValue (Scope *scope) {
      ilang::Variable * func = calling->Get(scope);
      std::list<ValuePass> par;
      for(Node * n : *params) {
	assert(dynamic_cast<parserNode::Value*>(n));
	par.push_back(dynamic_cast<parserNode::Value*>(n)->GetValue(scope));
      }
      boost::any_cast<Function*>(
				 //scope->lookup("something")
				 func
				 ->Get()->Get())->Call(scope->fileScope(), par);
      
      // needs to be changed to return the value
      
    }
    PrintCall::PrintCall(list<Node*> *args):
      Call(NULL, args) {}
    ValuePass PrintCall::GetValue (Scope *scope) {
      debug("made into the print Call");
      //for(list<Node*>::iterator it = params->begin(), end = params->end(); it != end; it++) {
      for(Node *it : *params) {
	assert(dynamic_cast<parserNode::Value*>(it));
	dynamic_cast<parserNode::Value*>((it))->GetValue(scope)->Print();
      }
      debug("made it out of print");
    }

    AssignExpr::AssignExpr (Variable *t, Value *v):target(t), eval(v) {
      assert(eval);
      assert(target);
    }
    void AssignExpr::Run (Scope *scope) {

      ValuePass v = eval->GetValue(scope);

      target->Set(scope, v);
      scope->Debug();
      //return RunReturn(new ilang::Value);
      //ilang::Variable *var = target->Get();
    }
  }
}
