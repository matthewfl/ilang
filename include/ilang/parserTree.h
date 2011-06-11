#ifndef _ilang_parserTree
#define _ilang_parserTree

#include <list>
#include <string>

#include <boost/shared_ptr.hpp>

#include "variable.h"

namespace ilang {
  class Scope;
  namespace parserNode {
    using std::list;
    using boost::shared_ptr;
    typedef boost::shared_ptr<ilang::Value> ValueReturn;
    class Node {
    public:
      virtual void Run()=0;
      virtual void Scope(Scope*)=0;
      void randomsdafasdf(){}
    };

    class Head {
    private:
      std::list<Node*> *Declars;
    public:
      Head(std::list<Node*>*);
      void Run();
      void Scope();
    };


    class Expression : public Node {
    };

    class Value : public Node {
    public:
      virtual ValueReturn GetValue()=0;
      void Scope(Scope *p) {}
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
      void Scope(Scope*);
    };
    class WhileStmt : public Node {
    public:
      WhileStmt();
      void Run();
      void Scope(Scope*);
    };
    class ForStmt : public Node {
    public:
      ForStmt();
      void Run();
      void Scope(Scope*);
    };
    
    class Function : public Value {
    private:
      std::list<Node*> *body;
      std::list<Node*> *params;
    public:
      Function(std::list<Node*> *p, std::list<Node*> *b); 
      void Run();
      void Call();
      ValueReturn GetValue();
      void Scope(Scope*);
    };

    class Variable : public Node {
    private:
      std::list<std::string> *name;
      std::list<std::string> *modifiers;
    public:
      Variable (std::list<std::string> *n, std::list<std::string> *mod);
      void Run();
      void Set(ilang::Variable *var);
      ilang::Variable Get();
      void Scope(Scope*);
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
      void Scope(Scope*);
    };
    class PrintCall : public Call {
    public:
      PrintCall(std::list<Node*> *args);
      ValueReturn GetValue (); // returns null
      void Scope(Scope*);
    };

    
    class AssignExpr : public Expression {
    private:
      Variable *target;
      Value *eval;
    public:
      AssignExpr (Variable *target, Value *value);
      void Run();
      void Scope(Scope*);
    };
  }
}


#endif // _ilang_parserTree
