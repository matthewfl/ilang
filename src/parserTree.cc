#include "parserTree.h"

#include <iostream>
using namespace std;

namespace ilang {
  namespace parserNode {
    void Function::Run() {}
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
    Variable::Variable (string n, list<string> *modifiers):
      name(n) {
      cout << "\t\t\t" << name << endl;
    }
    void Variable::Run () {
      cout << "\t\t\tSetting variable: " << name << endl;
    }
    void Variable::Set (ilang::Variable *var) {
      
    }
    Call::Call () {
      
    }
    void Call::Run() {
    }

    AssignExpr::AssignExpr (Variable *target, Value *value) {
      target->Run();
    }
    void AssignExpr::Run () {
      //target->Set(value->Run())
    }
  }
}
