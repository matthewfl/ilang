#ifndef _ilang_parserTree
#define _ilang_parserTree

#include <list>
#include <string>

namespace ilang {
  namespace parserNode {
    using std::list;
    class Node {
    public:
      virtual void Run()=0;
    };

    class Head {
      
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
    
    class Function : public Node {
    public:
      Function();
      void Run();
      void Call();
    };
    class Variable : public Node {
    public:
      void Run();
    };
    class Call : public Node {
    public:
      Call();
      void Run();
    };
  }
}


#endif // _ilang_parserTree
