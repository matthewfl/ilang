#ifndef _ilang_function
#define _ilang_function

// TODO: make this functions created using this interface so that the system will have an easily way to create functions that are called rather than having

#include <vector>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "variable.h"
#include "scope.h"
#include "parserTree.h"


namespace ilang {
  //class Scope;
  // First, scope that is running in, arguments passed, return value
  typedef boost::function<void (Scope*, std::vector<ValuePass>, ValuePass*)> Function_ptr;
  /*
  class Function {
    Scope *bound_scope;
  public:
    Function (ilang::parserNode::Function *fun);
    Function (Function_ptr fun);
    void BindScope(Scope*);
    ValuePass Call(vector<ilang::Value*> &p);
  };
  */
} // namespace ilang


#endif // _ilang_function
