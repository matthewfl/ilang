#include "parserTree.h"
#include "parser.h"
#include "scope.h"
#include "debug.h"

#include <iostream>
using namespace std;

namespace ilang {
  namespace parserNode {
    Head::Head(list<Node*> *declars): Declars(declars) {
      debug(4, "head " << declars->size() );
      debug(4, "running ++++++++++++++++++++++++++++++++++++++++++++" );
      Run();
    }
    void Head::Run () {
      FileScope scope;
      for(list<Node*>::iterator it = Declars->begin(); it !=  Declars->end(); it++) {
	debug(5, "calling run" )
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
      debug(5, "string get value" );
      return ValuePass(new ilang::Value(std::string(string)));
    }
    
  
    void ForStmt::Run(Scope *scope) {}



    IfStmt::IfStmt (Node *test_, Node* True_, Node* False_): True(True_), False(False_) {
      Value *t = dynamic_cast<Value*>(test_);
      assert(t);
      test=t;
    }
    void IfStmt::Run(Scope *scope) {
      if(test->GetValue(scope)->isTrue()) {
	if(True) True->Run(scope);
      }else{
	if(False) False->Run(scope);
      }
    }
    ForStmt::ForStmt () {

    }
    WhileStmt::WhileStmt (Node *test_, Node *exe_): exe(exe_) {
      Value *t=dynamic_cast<Value*>(test);
      assert(t);
      test=t;
    }
    void WhileStmt::Run(Scope *scope) {
      while(test->GetValue(scope)->isTrue())
	exe->Run(scope);
    }

    Function::Function (list<Node*> *p, list<Node*> *b):body(b), params(p) {
      debug(5, "\t\t\tfunction constructed" );

    }
    void Function::Run(Scope *scope) {
      list<ilang::Value*> p;
      Call(scope, p);
    }
    ValuePass Function::GetValue(Scope *scope) {
      debug(5, "Function get value");
      return ValuePass(new ilang::Value(this));
    }
    void Function::Call(list<ilang::Value*> params) {
      // this needs to be removed
      Call(NULL, params);
    }
    void Function::Call(Scope *_scope, list<ilang::Value*> p) {
      FunctionScope scope(_scope);
      debug(5,"function called");
      if(params) { // the parser set params to NULL when there are non
	list<Node*>::iterator it = params->begin();
	for(ilang::Value * v : p) {
	  if(it==params->end()) break;
	  assert(dynamic_cast<parserNode::Variable*>(*it));
	  assert(v);
	  dynamic_cast<parserNode::Variable*>(*it)->Set(&scope, ValuePass(v));
	  it++;
	}
      }
      for(Node *n : *body) {
	assert(n);
	debug(5,"function run");
	n->Run(&scope);
      }
      
    }

    Variable::Variable (list<string> *n, list<string> *mod):
      name(n), modifiers(mod) {
      //cout << "\t\t\t" << name << "\n";
    }
    void Variable::Run (Scope *scope) {
      debug(4,"\t\t\tSetting variable: " << name->front());
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
      debug(4,"Set: " << name->front() << " " << var << " " << v->Get());
      scope->Debug();
    }
    ilang::Variable * Variable::Get(Scope *scope) {
      scope->Debug();
      ilang::Variable *v;
      v = scope->lookup(name->front());
      assert(v);
      debug(4,"Get: " << name->front() << " " << v->Get());
      return v;
    }
    Call::Call (Variable *call, list<Node*> *args):
      calling(call), params(args) {
      debug(4,"\t\t\tCalling function");
    }
    void Call::Run(Scope *scope) {
      debug(4,"call run");
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
      debug(5,"made into the print Call");
      //for(list<Node*>::iterator it = params->begin(), end = params->end(); it != end; it++) {
      for(Node *it : *params) {
	assert(dynamic_cast<parserNode::Value*>(it));
	dynamic_cast<parserNode::Value*>((it))->GetValue(scope)->Print();
      }
      debug(5,"made it out of print");
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
    MathEquation::MathEquation(Value *r, Value *l, action a) : left(l), right(r), Act(a) {}
    void MathEquation::Run(Scope *scope) {
      switch(Act) {
      case add:
      case subtract:
      case multiply:
      case devide:
      }
    }
    ValuePass MathEquation::GetValue(Scope *scope) {
      switch(Act) {
      case add:
	return 
      case subtract:
      case multiply:
      case devide:
      }
    }
  }
}
