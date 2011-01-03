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
    class Head {
      
    };
  }
}


#endif // _ilang_parserTree
