#ifndef _ilang_variable
#define _ilang_variable

#include <map>
#include <string>
#include <list>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

namespace ilang {
  class Value;
  //typedef boost::shared_ptr<ilang::Value> ValuePass;
  typedef ilang::Value* ValuePass;
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
    ValuePass val;
    Variable()=delete;
  public:
    Variable (std::string name, std::list<std::string> modifiers);
    void Set(ValuePass v);
    Value * Get();
  };

  // a wrapper class for any numerical value
  class Number {
  private:
    enum Type {
      null,
      Int,
      Double
    };
    Type type;
    union {
      long INT;
      double DOUBLE;
    };
  public:
    Number(){type=null;}
    void Set();
  };
  
  class Value {
  private:
    boost::any val;
    friend class Variable;
  public:
    Value (boost::any);
    Value ();
    void Print();
    bool isTrue();
    boost::any & Get();
  };
  

}

#define ILANG_VARIABLE_MODIFIER(name, obj)				\
  namespace { struct _ILANG_VAR_MOD_##name {				\
    _ILANG_VAR_MOD_##name () {						\
      ::ilang::ilang_Variable_modifier_list[ #name ] = new obj ;	\
    }} _ILANG_VAR_MOD_##name##_run;					\
  }

 
#endif // _ilang_variable
