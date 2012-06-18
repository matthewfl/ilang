#ifndef _ilang_function
#define _ilang_function

// TODO: make this functions created using this interface so that the system will have an easily way to create functions that are called rather than having

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "variable.h"
#include "scope.h"

namespace ilang {
  //class Scope;
  typedef boost::function<ValuePass (Scope*, std::list<ValuePass>, ValuePass*)> Function_ptr;
  class Function {

  };
} // namespace ilang


#endif // _ilang_function
