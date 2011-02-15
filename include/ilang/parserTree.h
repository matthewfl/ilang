#ifndef _ilang_parserTree
#define _ilang_parserTree

#include <list>
#include <string>

namespace ilang {
  class Variable;
  namespace parserNode {
    using std::list;
    class Node {
    public:
      virtual void Run()=0;
    };

    class Head {
      
    };


    class Expression : public Node {
    };

    class Value : public Node {
    
    };
    
    class IfStmt : public Node {
    public:
      IfStmt();
      void Run();
    };
    class WhileStmt : public Node {
    public:
      WhileStmt();
      void Run();
    };
    class ForStmt : public Node {
    public:
      ForStmt();
      void Run();
    };
    
    class Function : public Value {
    public:
      Function();
      void Run();
      void Call();
    };
    class Variable : public Node {
    private:
      std::string name;
    public:
      Variable (std::string n, std::list<std::string> *modifiers);
      void Run();
      void Set(ilang::Variable *var);
      ilang::Variable Get();
    };
    class Call : public Node {
    public:
      Call();
      void Run();
    };

    
    class AssignExpr : public Expression {
    public:
      AssignExpr (Variable *target, Value *value);
      void Run();
    };
  }
}


#endif // _ilang_parserTree
