#ifndef _ilang_parserTree
#define _ilang_parserTree

#include <list>
#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include "variable.h"
#include "scope.h"
#include "import.h"

namespace ilang {
  class Scope;
  class Object;
  class Class;
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
      FileScope *scope;
      ImportScopeFile *Import;
      std::list<Node*> *Declars;
    public:
      Head(std::list<Node*>*, ImportScopeFile*);
      void Link();
      void Run();
      Scope *GetScope ();
    };


    
    class Value : public Node {
    public:
      virtual ValuePass GetValue(Scope*)=0;
      virtual ValuePass CallFun(Scope*, std::vector<ValuePass> &par); // I feel a little funny about having this here, but this should be the right place
    };
    class Constant : public Value {
    public:
      void Run(Scope*);
    };

    

    // I guess we will leave this in, but most things are using Value, not expression
    class Expression : public Value {
    };

    class Variable;

    class Object : public Value {
    private:
      std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *objects;
    public:
    Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
    };

    class Class : public Value {
    private:
      std::list<Node*> *parents;
      std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *objects;
    public:
      Class(std::list<Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
    };

    class Array : public Value {
    private:
      std::list<Node*> *elements;
      std::list<std::string> *modifiers;
    public:
      Array(std::list<Node*> *e, std::list<std::string> *m);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
    };

    class StringConst : public Constant {
    private:
      //char *string;
      std::string string;
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
    private:
      Node *pre;
      Value *test;
      Node *each;
      Node *exe;
    public:
      ForStmt(Node*, Node*, Node*, Node*);
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
      //void Call(std::vector<ilang::Value*>);
      void Call(Scope *_scope_made, Scope *_scope_self, std::vector<ValuePass>&, ValuePass *_ret=NULL);
      ValuePass GetValue(Scope*);
    };

    

    class Variable : public Value {
    private:
      friend class Variable_compare;
      friend class ::ilang::Object;
      friend class ::ilang::Class;

      std::list<std::string> *name;
      std::list<std::string> *modifiers;
    public:
      Variable (std::list<std::string> *n, std::list<std::string> *mod);
      void Run(Scope*);
      void Set(Scope*, ValuePass var, bool force = false);
      // not sure if I want to make this virtual, but I believe that this will be the most effective way to make this work easily
      virtual ilang::Variable * Get(Scope*);
      ValuePass GetValue(Scope*);
      virtual std::string GetFirstName();
      //virtual ValuePass CallFun (Scope*, std::vector<ValuePass> &par);
    };

    class Variable_compare {
    public:
      bool operator() (Variable* a, Variable *b);
    };


    class FieldAccess : public Variable {
    private:
      std::string identifier;
      Value *Obj;
    public:
      FieldAccess(Node*, std::string);
      //void Run(Scope*);
      //void Set(Scope*, ValuePass var);
      ilang::Variable * Get(Scope*);
      //ValuePass GetValue(Scope*);
      virtual std::string GetFirstName();
      virtual ValuePass CallFun (Scope*, std::vector<ValuePass> &par);
    };

    class ArrayAccess : public Variable {
      private:
      Value *Obj;
      Value *Lookup;
    public:
      ArrayAccess(Node*, Node*);
      ilang::Variable * Get(Scope*);
    };

    class Call : public Value {
    private:
      Value *calling;
    protected:
      std::list<Node*> *params;
    public:
      Call(Value *call, std::list<Node*> *args);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
    };
    class PrintCall : public Call {
    public:
      PrintCall(std::list<Node*> *args);
      ValuePass GetValue (Scope*); // returns null
    };
    class NewCall : public Call {
    public:
      NewCall(std::list<Node*> *args);
      ValuePass GetValue(Scope*); 
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
