#ifndef _ilang_parserTree
#define _ilang_parserTree

#include <deque>

namespace ilang {
  namespace parserNode {
    template <typename T> class List {
    private:
      std::deque<T*> holder;
    public:
      List(T* first) { holder.push_back(first); }
      
    };

    class Node {
    public:
      virtual void Run();
    };

    class Head {
      
    };
    
    class IfStmt : public Node {
    
    };
    class WhileStmt : public Node {
    
    };
    class ForStmt : public Node {
    
    };
    
    class Function : public Node {
      
    };
    class Variable : public Node {
      
    };
  }
}


#endif // _ilang_parserTree
