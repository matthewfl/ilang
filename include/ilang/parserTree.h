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
    //typedef boost::shared_ptr<ilang::Value> ValuePass;
    using ilang::ValuePass; // defined in variable.h
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


    
    class Value : public Node {
    public:
      virtual ValuePass GetValue(Scope*)=0;
    };
    class Constant : public Value {
    public:
      void Run(Scope*);
    };

    // I guess we will leave this in, but most things are using Value, not expression
    class Expression : public Value {
    };

    class StringConst : public Constant {
    private:
      char *string;
    public:
      StringConst(char *str);
      ValuePass GetValue(Scope*);
    };

    class IntConst : public Constant {
    private:
      long num;
    public:
      IntConst(long n);
      ValuePass GetValue(Scope*);
    };
    class FloatConst : public Constant {
    private:
      double num;
    public:
      FloatConst(double d);
      ValuePass GetValue(Scope*);
    };


    class IfStmt : public Node {
    private:
      Value *test;
      Node *True, *False;
    public:
      IfStmt(Node*, Node*, Node*);
      void Run(Scope*);
    };
    class WhileStmt : public Node {
    private:
      Value *test;
      Node *exe;
    public:
      WhileStmt(Node*, Node*);
      void Run(Scope*);
    };
    class ForStmt : public Node {
    public:
      ForStmt();
      void Run(Scope*);
    };

    class ReturnStmt : public Node {
    private:
      Value *ret;
    public:
      ReturnStmt(Node*);
      void Run(Scope*);
    };
    
    class Function : public Value {
    private:
      std::list<Node*> *body;
      std::list<Node*> *params;
    public:
      Function(std::list<Node*> *p, std::list<Node*> *b); 
      void Run(Scope*);
      void Call(std::list<ilang::Value*>);
      void Call(Scope*, std::list<ilang::Value*>&, ValuePass *_ret=NULL);
      ValuePass GetValue(Scope*);
    };

    class Variable : public Node {
    private:
      std::list<std::string> *name;
      std::list<std::string> *modifiers;
    public:
      Variable (std::list<std::string> *n, std::list<std::string> *mod);
      void Run(Scope*);
      void Set(Scope*, ValuePass var);
      ilang::Variable * Get(Scope*);
    };
    class Call : public Value {
    private:
      Variable *calling;
    protected:
      std::list<Node*> *params;
    public:
      Call(Variable *call, std::list<Node*> *args);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
    };
    class PrintCall : public Call {
    public:
      PrintCall(std::list<Node*> *args);
      ValuePass GetValue (Scope*); // returns null
    };

    
    class AssignExpr : public Expression {
    private:
      Variable *target;
      Value *eval;
    public:
      AssignExpr (Variable *target, Value *value);
      void Run(Scope*);
      ValuePass GetValue(Scope *scope);
    };
    class MathEquation : public Expression {
    public:
      enum action {
	add,
	subtract,
	multiply,
	devide
      };
      MathEquation(Value *l, Value *r, action a);
      void Run(Scope *scope);
      ValuePass GetValue(Scope *scope);
    private:
      Value *left, *right;
      action Act;
    };
    class LogicExpression : public Expression {
    public:
      enum action {
	Equal,
	Not_Equal,
	Less_Equal,
 	Greater_Equal,
	Less,
	Greater,
	And,
	Or
      };
      LogicExpression(Value *l, Value *r, action a);
      void Run(Scope *scope);
      ValuePass GetValue(Scope *scope);
    private:
      Value *left, *right;
      action Act;
    };
  }
}


#endif // _ilang_parserTree
