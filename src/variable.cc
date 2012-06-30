#include "variable.h"
#include "debug.h"

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
  Variable_modifier * Variable_modifier::new_variable(std::string name, Variable *v) {
    return this;
  }
 
  Variable::Variable(string name, list<string> modifiers) {
    Name = name;
    for(list<string>::iterator it=modifiers.begin(); it!=modifiers.end(); it++) {
      Variable_modifier *m = ilang_Variable_modifier_list[*it];
      if(!m)
	cerr << "Variable modifier "<<*it<<" not found\n";
      else
	Modifiers.push_back(m->new_variable(name, this));
    }
  }
  void Variable::Set(ValuePass v) {
    // this most likely will have a lot of calling of constructors and stuff for the shared_ptr
    val=v;
    debug(4, v << " " << val );
  }
  Value *  Variable::Get () {
    return val;
    //return val.get();
  }

  boost::any & Value::Get() {
    return val;
  }

  Value::Value(boost::any v): val(v) {}
  Value::Value(): val(NULL){}
  void Value::Print () {
    // catch the type id to make this faster
    //cout << "inside print " << this << endl;
    if(val.empty()) {
      cout << "printing an empty variable\n";
    }else if(typeid(std::string) == val.type()) {
      cout << boost::any_cast<std::string>(val);
    }else if(typeid(long) == val.type()) {
      cout << boost::any_cast<long>(val);
    }else if(typeid(int) == val.type()) {
      // this should not happen as all ints are currently long
      cout << boost::any_cast<int>(val);
    }else if(typeid(double) == val.type()) {
      cout << boost::any_cast<double>(val);
    }else{
      cout << "could not figure out type: "<< val.type().name() << endl;
    }
    cout <<  flush;
    //cout << "over the print\n";
  }
  bool Value::isTrue () {
    if(typeid(bool) == val.type()) {
      return boost::any_cast<bool>(val);
    }else if(typeid(std::string) == val.type()) {
      return boost::any_cast<std::string>(val) != "";
    }else if(typeid(long) == val.type()) {
      return boost::any_cast<long>(val) != 0;
    }else if(typeid(int) == val.type()) {
      return boost::any_cast<int>(val) != 0;
    }else if(typeid(double) == val.type()) {
      return boost::any_cast<double>(val) != 0;
    }else{
      return true; // I guess having a value, even if we can't figure out what it is makes it true
    }
  }
}


namespace {
  using namespace ilang;
  class Int_var_type : public ilang::Variable_modifier {
  public:
    bool Check (const boost::any &val) {
      return typeid(int) == val.type() || typeid(long) == val.type();
    }
  };
  ILANG_VARIABLE_MODIFIER(int, Int_var_type)
  class String_var_type : public ilang::Variable_modifier {
  public:
    bool Check (const boost::any &val) {
      return typeid(std::string) == val.type();
      return false;
    }
  };
  ILANG_VARIABLE_MODIFIER(string, String_var_type)
}
