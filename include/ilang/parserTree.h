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
    typedef boost::shared_ptr<Value> RunReturn;
    class Node {
    public:
      virtual RunReturn Run()=0;
    };

    class Head {
      
    };


    class Expression : public Node {
    };

    class Value : public Node {
    
    };
    class Constant : public Value {
    
    };

    class StringConst : public Constant {
    private:
      char *string;
    public:
      StringConst(char *str);
      RunReturn Run();
    };


    class IfStmt : public Node {
    public:
      IfStmt();
      RunReturn Run();
    };
    class WhileStmt : public Node {
    public:
      WhileStmt();
      RunReturn Run();
    };
    class ForStmt : public Node {
    public:
      ForStmt();
      RunReturn  Run();
    };
    
    class Function : public Value {
    public:
      Function();
      RunReturn Run();
      void Call();
    };

    class Variable : public Node {
    private:
      std::list<std::string> *name;
    public:
      Variable (std::list<std::string> *n, std::list<std::string> *modifiers);
      RunReturn Run();
      void Set(ilang::Variable *var);
      ilang::Variable Get();
    };
    class Call : public Value {
    private:
      Variable *calling;
    public:
      Call(Variable *call);
      RunReturn Run();
    };

    
    class AssignExpr : public Expression {
    public:
      AssignExpr (Variable *target, Value *value);
      RunReturn Run();
    };
  }
}


#endif // _ilang_parserTree
