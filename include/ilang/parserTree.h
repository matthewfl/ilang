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
      virtual void Run(Scope*)=0;
      void randomsdafasdf(){} // take this out eventually
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
      virtual ValueReturn GetValue(Scope*)=0;
    };
    class Constant : public Value {
      
    };

    class StringConst : public Constant {
    private:
      char *string;
    public:
      StringConst(char *str);
      void Run(Scope*);
      ValueReturn GetValue(Scope*);
    };


    class IfStmt : public Node {
    public:
      IfStmt();
      void Run(Scope*);
    };
    class WhileStmt : public Node {
    public:
      WhileStmt();
      void Run(Scope*);
    };
    class ForStmt : public Node {
    public:
      ForStmt();
      void Run(Scope*);
    };
    
    class Function : public Value {
    private:
      std::list<Node*> *body;
      std::list<Node*> *params;
    public:
      Function(std::list<Node*> *p, std::list<Node*> *b); 
      void Run(Scope*);
      void Call(std::list<ilang::parserNode::Value*>);
      ValueReturn GetValue(Scope*);
    };

    class Variable : public Node {
    private:
      std::list<std::string> *name;
      std::list<std::string> *modifiers;
    public:
      Variable (std::list<std::string> *n, std::list<std::string> *mod);
      void Run(Scope*);
      void Set(Scope*, ilang::Variable *var);
      ilang::Variable Get(Scope*);
    };
    class Call : public Value {
    private:
      Variable *calling;
    protected:
      std::list<Node*> *params;
    public:
      Call(Variable *call, std::list<Node*> *args);
      void Run(Scope*);
      ValueReturn GetValue(Scope*);
    };
    class PrintCall : public Call {
    public:
      PrintCall(std::list<Node*> *args);
      ValueReturn GetValue (Scope*); // returns null
    };

    
    class AssignExpr : public Expression {
    private:
      Variable *target;
      Value *eval;
    public:
      AssignExpr (Variable *target, Value *value);
      void Run(Scope*);
    };
  }
}


#endif // _ilang_parserTree
