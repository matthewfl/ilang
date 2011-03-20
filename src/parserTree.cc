#include "parserTree.h"

#include <iostream>
using namespace std;

namespace ilang {
  namespace parserNode {
    StringConst::StringConst(char *str) :string(str){
      cout << string << endl;
    }
    RunReturn StringConst::Run() {}
    RunReturn Function::Run() {}
    void Function::Call() {}
    RunReturn IfStmt::Run() {}
    RunReturn WhileStmt::Run() {}
    RunReturn ForStmt::Run() {}



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
    RunReturn Variable::Run () {
      cout << "\t\t\tSetting variable: " << name->front() << endl;
    }
    void Variable::Set (ilang::Variable *var) {
      
    }
    Call::Call (Variable *call):
      calling(call) {
      cout << "\n\t\t\tCalling function \n";
    }
    RunReturn Call::Run() {
      
    }

    AssignExpr::AssignExpr (Variable *target, Value *value) {
      target->Run();
    }
    RunReturn  AssignExpr::Run () {
      //target->Set(value->Run())
    }
  }
}
