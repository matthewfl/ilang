#ifndef _ilang_parserTree
#define _ilang_parserTree

#include <list>
#include <string>

#include <boost/shared_ptr.hpp>

#include "variable.h"

namespace ilang {
  namespace parserNode {
    using std::list;
    using boost::shared_ptr;
    typedef boost::shared_ptr<ilang::Value> ValueReturn;
    class Node {
    public:
      virtual void Run()=0;
      void randomsdafasdf(){}
    };

    class Head {
    private:
      std::list<Node*> *Declars;
    public:
      Head(std::list<Node*>*);
      void Run();
    };


    class Expression : public Node {
    };

    class Value : public Node {
    public:
      virtual ValueReturn GetValue()=0;
    };
    class Constant : public Value {
      
    };

    class StringConst : public Constant {
    private:
      char *string;
    public:
      StringConst(char *str);
      void Run();
      ValueReturn GetValue();
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
      ValueReturn GetValue();
    };

    class Variable : public Node {
    private:
      std::list<std::string> *name;
    public:
      Variable (std::list<std::string> *n, std::list<std::string> *modifiers);
      void Run();
      void Set(ilang::Variable *var);
      ilang::Variable Get();
    };
    class Call : public Value {
    private:
      Variable *calling;
    protected:
      std::list<Node*> *params;
    public:
      Call(Variable *call, std::list<Node*> *args);
      void Run();
      ValueReturn GetValue();
    };
    class PrintCall : public Call {
    public:
      PrintCall(std::list<Node*> *args);
      ValueReturn GetValue (); // returns null
    };

    
    class AssignExpr : public Expression {
    private:
      Variable *target;
      Value *eval;
    public:
      AssignExpr (Variable *target, Value *value);
      void Run();
    };
  }
}


#endif // _ilang_parserTree
