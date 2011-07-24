#include "variable.h"
#include <iostream>

using namespace std;

namespace ilang {
  map<string, Variable_modifier*> ilang_Variable_modifier_list;

  bool Variable::Check (boost::any &a) {
    for(list<Variable_modifier*>::iterator it=Modifiers.begin(); it!=Modifiers.end(); it++) {
      if(!(*it)->Check(a))
	return false;
    }
    return true;
  }
  Variable::Variable(string name, list<string> modifiers) {
    Name = name;
    for(list<string>::iterator it=modifiers.begin(); it!=modifiers.end(); it++) {
      Variable_modifier *m = ilang_Variable_modifier_list[*it];
      if(!m)
	cerr << "Variable modifier "<<*it<<" not found\n";
      else
	Modifiers.push_back(m);
    }
  }
  void Variable::Set(ValuePass v) {
    // this most likely will have a lot of calling of constructors and stuff for the shared_ptr
    val=v;
  }
  Value *  Variable::Get () {
    return val;
    //return val.get();
  }

  boost::any Value::Get() {
    return val;
  }

  Value::Value(boost::any v): val(v) {}
  Value::Value(){}
  void Value::Print () {
    if(typeid(std::string) == val.type()) {
      cout << boost::any_cast<std::string>(val);
    }else if(typeid(int) == val.type()) {
      cout << boost::any_cast<int>(val);
    }else{
      cout << "could not figure out type: "<< val.type().name() << endl;
    }
  }
}


namespace {
  using namespace ilang;
  class Int_var_type : public ilang::Variable_modifier {
  public:
    bool Check (const boost::any &a) {
      return true;
    }
  };
  ILANG_VARIABLE_MODIFIER(int, Int_var_type)
  class String_var_type : public ilang::Variable_modifier {
  public:
    bool Check (const boost::any &a) {
      return false;
    }
  };
  ILANG_VARIABLE_MODIFIER(string, String_var_type)
}
