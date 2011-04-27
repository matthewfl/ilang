#include "parserTree.h"
#include "parser.h"

#include <iostream>
using namespace std;

namespace ilang {
  namespace parserNode {
    Head::Head(list<Node*> *declars): Declars(declars) {
      cout << "head " << declars->size() << endl;
      Run();
    }
    void Head::Run () {
      for(list<Node*>::iterator it = Declars->begin(); it !=  Declars->end(); it++) {
	(*it)->Run();
      }
      //return RunReturn(new ilang::Value);
    }


    StringConst::StringConst(char *str) :string(str){}
    void StringConst::Run() {
      //return RunReturn(new ilang::Value(string));
    }
    ValueReturn StringConst::GetValue () {
      return ValueReturn(new ilang::Value(string));
    }
    void Function::Run() {}
    ValueReturn Function::GetValue() {}
    void Function::Call() {}
    void IfStmt::Run() {}
    void WhileStmt::Run() {}
    void ForStmt::Run() {}



    IfStmt::IfStmt () {
      
    }
    ForStmt::ForStmt () {
      
    }
    WhileStmt::WhileStmt () {
      
    }

    Function::Function () {
      cout << "\t\t\tfunction constructed\n";
      
    }
    Variable::Variable (list<string> *n, list<string> *modifiers):
      name(n) {
      //cout << "\t\t\t" << name << "\n";
    }
    void Variable::Run () {
      cout << "\t\t\tSetting variable: " << name->front() << endl;
    }
    void Variable::Set (ilang::Variable *var) {
      
    }
    ilang::Variable Get() {
      
    }
    Call::Call (Variable *call, list<Node*> *args):
      calling(call), params(args) {
      cout << "\n\t\t\tCalling function \n";
    }
    void Call::Run() {
      GetValue();
    }
    ValueReturn Call::GetValue () {}
    PrintCall::PrintCall(list<Node*> *args):
      Call(NULL, args) {}
    ValueReturn PrintCall::GetValue () {
      for(list<Node*>::iterator it = params->begin(), end = params->end(); it != end; it++) {
	
      }
    }

    AssignExpr::AssignExpr (Variable *t, Value *v):target(t), eval(v) {
    
    }
    void AssignExpr::Run () {
      //target->Set(value->Run())
      
      //return RunReturn(new ilang::Value);
      //ilang::Variable *var = target->Get();
    }
  }
}
