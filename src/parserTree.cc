#include "parserTree.h"
#include "parser.h"
#include "scope.h"

#include <iostream>
using namespace std;

namespace ilang {
  namespace parserNode {
    Head::Head(list<Node*> *declars): Declars(declars) {
      cout << "head " << declars->size() << endl;
      Run();
    }
    void Head::Run () {
      FileScope scope;
      for(list<Node*>::iterator it = Declars->begin(); it !=  Declars->end(); it++) {
	cout << "calling run\n";
	(*it)->Run(&scope);
      }
      list<ilang::Value*> v;
      boost::any_cast<Function*>(scope.lookup("main")->Get()->Get())->Call(v);
      //return RunReturn(new ilang::Value);
    }


    StringConst::StringConst(char *str) :string(str){}
    void StringConst::Run(Scope *scope) {
      //return RunReturn(new ilang::Value(string));
    }
    ValuePass StringConst::GetValue (Scope *scope) {
      cout << "string get value\n";
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
      cout << "\t\t\tfunction constructed\n";

    }
    void Function::Run(Scope *scope) {
      list<ilang::Value*> p;
      Call(scope, p);
    }
    ValuePass Function::GetValue(Scope *scope) {
      cout << "Function get value\n";
      return ValuePass(new ilang::Value(this));
    }
    void Function::Call(list<ilang::Value*> params) {
      Call(NULL, params);
    }
    void Function::Call(Scope *_scope, list<ilang::Value*> p) {
      FunctionScope scope(_scope);
      cout << "function called\n" << flush;
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
	n->Run(&scope);
      }
      
    }

    Variable::Variable (list<string> *n, list<string> *mod):
      name(n), modifiers(mod) {
      //cout << "\t\t\t" << name << "\n";
    }
    void Variable::Run (Scope *scope) {
      cout << "\t\t\tSetting variable: " << name->front() << endl;
    }
    void Variable::Set (Scope *scope, ValuePass var) {
      ilang::Variable *v;
      if(!modifiers->empty())
	v = scope->forceNew(name->front(), *modifiers);
      else
	v = scope->lookup(name->front());
      assert(v);
      v->Set(var);
    }
    ilang::Variable Get(Scope *scope) {

    }
    Call::Call (Variable *call, list<Node*> *args):
      calling(call), params(args) {
      cout << "\n\t\t\tCalling function \n";
    }
    void Call::Run(Scope *scope) {
      GetValue(scope);
    }
    ValuePass Call::GetValue (Scope *scope) {}
    PrintCall::PrintCall(list<Node*> *args):
      Call(NULL, args) {}
    ValuePass PrintCall::GetValue (Scope *scope) {
      cout << "made into the print Call\n";
      //for(list<Node*>::iterator it = params->begin(), end = params->end(); it != end; it++) {
      for(Node *it : *params) {
	dynamic_cast<parserNode::Value*>((it))->GetValue(scope)->Print();
      }
    }

    AssignExpr::AssignExpr (Variable *t, Value *v):target(t), eval(v) {
      assert(eval);
      assert(target);
    }
    void AssignExpr::Run (Scope *scope) {

      ValuePass v = eval->GetValue(scope);

      target->Set(scope, v);

      //return RunReturn(new ilang::Value);
      //ilang::Variable *var = target->Get();
    }
  }
}
