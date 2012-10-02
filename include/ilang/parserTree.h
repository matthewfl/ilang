#ifndef _ilang_parserTree
#define _ilang_parserTree

#include <list>
#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include "variable.h"
#include "scope.h"
#include "import.h"
#include "print.h"

/* Need to figure out deleting of these objects
 * There are issues with things such as functions where the origional structure might be referenced
 * Classes and objects return a new type that contains all the necessary information
 * most likeley will need to create something similar for functions so that the parserTree can be deleted without the function getting messed up
 * also, functions accessing the scope that they are created in could cause a problem
 */

namespace ilang {
  class Scope;
  class Object;
  class Class;
  class Modification;
  namespace parserNode {
    using std::list;
    using boost::shared_ptr;
    //typedef boost::shared_ptr<ilang::Value> ValuePass;
    using ilang::ValuePass; // defined in variable.h
    class Node {
      friend class ilang::Modification;
    private:
      unsigned long _node_id;
    public:
      Node();
      const unsigned long getID() { return _node_id; }
      virtual void Run(Scope*)=0;
      void randomsdafasdf(){} // take this out eventually
      virtual void Print(Printer*) =0;
    };

    class Head {
      friend class ilang::Modification;
    private:
      FileScope *scope;
      ImportScopeFile *Import;
      std::list<Node*> *Declars;
    public:
      Head(std::list<Node*>*, ImportScopeFile*);
      void Link();
      void Run();
      Scope *GetScope ();
      void Print(Printer*);
    };



    class Value : public Node {
      friend class ilang::Modification;
    public:
      virtual ValuePass GetValue(Scope*)=0;
      virtual ValuePass CallFun(Scope*, std::vector<ValuePass> &par); // I feel a little funny about having this here, but this should be the right place
    };
    class Constant : public Value {
      friend class ilang::Modification;
    public:
      void Run(Scope*);
    };



    // I guess we will leave this in, but most things are using Value, not expression
    class Expression : public Value {
      friend class ilang::Modification;
    };

    class Variable;

    class Object : public Value {
      friend class ilang::Modification;
    private:
      std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *objects;
    public:
    Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
      void Print(Printer*);
    };

    class Class : public Value {
      friend class ilang::Modification;
    private:
      std::list<Node*> *parents;
      std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *objects;
    public:
      Class(std::list<Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
      void Print(Printer *p);
    };

    class Array : public Value {
      friend class ilang::Modification;
    private:
      std::list<Node*> *elements;
      std::list<std::string> *modifiers;
    public:
      Array(std::list<Node*> *e, std::list<std::string> *m);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
      void Print(Printer*);
    };

    class StringConst : public Constant {
      friend class ilang::Modification;
    private:
      //char *string;
      std::string string;
    public:
      StringConst(char *str);
      ValuePass GetValue(Scope*);
      void Print(Printer*);
    };

    class IntConst : public Constant {
      friend class ilang::Modification;
    private:
      long num;
    public:
      IntConst(long n);
      ValuePass GetValue(Scope*);
      void Print(Printer *p);
    };
    class FloatConst : public Constant {
      friend class ilang::Modification;
    private:
      double num;
    public:
      FloatConst(double d);
      ValuePass GetValue(Scope*);
      void Print(Printer*);
    };


    class IfStmt : public Node {
      friend class ilang::Modification;
    private:
      Value *test;
      Node *True, *False;
    public:
      IfStmt(Node*, Node*, Node*);
      void Run(Scope*);
      void Print(Printer*);
    };
    class WhileStmt : public Node {
      friend class ilang::Modification;
    private:
      Value *test;
      Node *exe;
    public:
      WhileStmt(Node*, Node*);
      void Run(Scope*);
      void Print(Printer*);
    };
    class ForStmt : public Node {
      friend class ilang::Modification;
    private:
      Node *pre;
      Value *test;
      Node *each;
      Node *exe;
    public:
      ForStmt(Node*, Node*, Node*, Node*);
      void Run(Scope*);
      void Print(Printer*);
    };

    class ReturnStmt : public Node {
      friend class ilang::Modification;
    private:
      Value *ret;
    public:
      ReturnStmt(Node*);
      void Run(Scope*);
      void Print(Printer*);
    };

    class Function : public Value {
      friend class ilang::Modification;
    private:
      std::list<Node*> *body;
      std::list<Node*> *params;
    public:
      Function(std::list<Node*> *p, std::list<Node*> *b);
      void Run(Scope*);
      //void Call(std::vector<ilang::Value*>);
      void Call(Scope *_scope_made, Scope *_scope_self, std::vector<ValuePass>&, ValuePass *_ret=NULL);
      ValuePass GetValue(Scope*);
      void Print(Printer*);
    };


    class Variable : public Value {
      friend class ilang::Modification;
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
      void Print(Printer*);
    };

    // TODO: take this out
    // this is currently not being used anymore
    // was planning at one point to use this for sorting variables
    class Variable_compare {
    public:
      bool operator() (Variable* a, Variable *b);
    };


    class FieldAccess : public Variable {
      friend class ilang::Modification;
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
      void Print(Printer*);
    };

    class ArrayAccess : public Variable {
      friend class ilang::Modification;
    private:
      Value *Obj;
      Value *Lookup;
    public:
      ArrayAccess(Node*, Node*);
      ilang::Variable * Get(Scope*);
      virtual std::string GetFirstName();
      void Print(Printer*);
    };

    class Call : public Value {
      friend class ilang::Modification;
    private:
      Value *calling;
    protected:
      std::list<Node*> *params;
    public:
      Call(Value *call, std::list<Node*> *args);
      void Run(Scope*);
      ValuePass GetValue(Scope*);
      void Print(Printer*);
    };
    class PrintCall : public Call {
      friend class ilang::Modification;
    public:
      PrintCall(std::list<Node*> *args);
      ValuePass GetValue (Scope*); // returns null
      void Print(Printer*);
    };
    class NewCall : public Call {
      friend class ilang::Modification;
    public:
      NewCall(std::list<Node*> *args);
      ValuePass GetValue(Scope*);
      void Print(Printer*);
    };

    class AssertCall : public Call {
      friend class ilang::Modification;
    private:
      int lineN;
      std::string fileName;
    public:
      AssertCall(int line, const char *name, std::list<Node*> *args);
      ValuePass GetValue(Scope*);
      void Print(Printer*);
    };


    class AssignExpr : public Expression {
      friend class ilang::Modification;
    private:
      Variable *target;
      Value *eval;
    public:
      AssignExpr (Variable *target, Value *value);
      void Run(Scope*);
      ValuePass GetValue(Scope *scope);
      void Print(Printer*);
    };
    class MathEquation : public Expression {
      friend class ilang::Modification;
    public:
      enum action {
	add,
	subtract,
	multiply,
	devide,
	uMinus
      };
      MathEquation(Value *l, Value *r, action a);
      void Run(Scope *scope);
      ValuePass GetValue(Scope *scope);
      void Print(Printer*);
    private:
      Value *left, *right;
      action Act;
    };
    class LogicExpression : public Expression {
      friend class ilang::Modification;
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
      void Print(Printer*);
    private:
      Value *left, *right;
      action Act;
    };
  }
}


#endif // _ilang_parserTree
