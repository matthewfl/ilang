#include "variable.h"

using namespace std;

namespace ilang {
  map<string, Variable_modifier*> ilang_Variable_modifier_list;
  
  bool Variable::Check (boost::any a) {
    for(list<Variable_modifier*>::iterator it=modifiers.begin(); it!=modifiers.end(); it++) {
      if(!(*it)->Check(a))
	return false;
    }
    return true;
  }
}


namespace {
  using namespace ilang;
  class Int_var_type : public Variable_modifier {
  public:
    bool Check (const boost::any &a) {
      return true;
    }
  };
  //ILANG_VARIABLE_MODIFIER(int, Int_var_type)
  class String_var_type : public Variable_modifier {
  public:
    bool Check (const boost::any &a) {
      return false;
    }
  };
  //ILANG_VARIABLE_MODIFIER(string, String_var_type)
}
