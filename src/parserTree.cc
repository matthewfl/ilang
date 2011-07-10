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
      //return RunReturn(new ilang::Value);
    }


    StringConst::StringConst(char *str) :string(str){}
    void StringConst::Run(Scope *scope) {
      //return RunReturn(new ilang::Value(string));
    }
    ValueReturn StringConst::GetValue (Scope *scope) {
      return ValueReturn(new ilang::Value(string));
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
      list<Value*> p;
      Call(p);
    }
    ValueReturn Function::GetValue(Scope *scope) {
      cout << "Function get value\n";
    }
    void Function::Call(list<Value*> params) {
      
    }

    Variable::Variable (list<string> *n, list<string> *mod):
      name(n), modifiers(mod) {
      //cout << "\t\t\t" << name << "\n";
    }
    void Variable::Run (Scope *scope) {
      cout << "\t\t\tSetting variable: " << name->front() << endl;
    }
    void Variable::Set (Scope *scope, ilang::Variable *var) {
      
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
    ValueReturn Call::GetValue (Scope *scope) {}
    PrintCall::PrintCall(list<Node*> *args):
      Call(NULL, args) {}
    ValueReturn PrintCall::GetValue (Scope *scope) {
      for(list<Node*>::iterator it = params->begin(), end = params->end(); it != end; it++) {
	dynamic_cast<Value*>((*it))->GetValue(scope)->Print();
      }
    }

    AssignExpr::AssignExpr (Variable *t, Value *v):target(t), eval(v) {
    
    }
    void AssignExpr::Run (Scope *scope) {
      eval->GetValue(scope);
      //target->Set(value->Run())
      
      //return RunReturn(new ilang::Value);
      //ilang::Variable *var = target->Get();
    }
  }
}
