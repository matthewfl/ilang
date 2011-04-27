#ifndef _ilang_variable
#define _ilang_variable

#include <map>
#include <string>
#include <list>
#include <boost/any.hpp>

namespace ilang {
  class Variable_modifier {
  public:
    virtual bool Check(const boost::any&)=0;
  };

  extern std::map<std::string, Variable_modifier*> ilang_Variable_modifier_list;


  class Variable {
  private:
    std::list<Variable_modifier*> Modifiers;
    std::string Name;
    bool Check (boost::any&);
  public:
    Variable (std::string name, std::list<std::string> modifiers);
  };
  
  class Value {
  private:
    boost::any val;
    friend class Variable;
  public:
    Value (boost::any);
    Value ();
  };
  

}

#define ILANG_VARIABLE_MODIFIER(name, obj)				\
  namespace { struct _ILANG_VAR_MOD_##name {				\
    _ILANG_VAR_MOD_##name () {						\
      ::ilang::ilang_Variable_modifier_list[ #name ] = new obj ;	\
    }} _ILANG_VAR_MOD_##name##_run;					\
  }

 
#endif // _ilang_variable
