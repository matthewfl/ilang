#ifndef _ilang_variable
#define _ilang_variable

#include <map>
#include <string>
#include <list>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

namespace ilang {
  using boost::shared_ptr;

  class Modification;
  class Value;
  class Variable;
  typedef boost::shared_ptr<ilang::Value> ValuePass;
  //typedef ilang::Value* ValuePass;
  class Variable_modifier {
  public:
    virtual bool Check(Variable *self, const boost::any&)=0;
    virtual shared_ptr<Variable_modifier> new_variable(Variable *self, std::string);
    virtual void Set(Variable *self, const boost::any &a) {}
    virtual void Read(Variable *self, ValuePass &val) {}
    virtual const char* Name() { return "NOT SET"; }
  };

  extern std::map<std::string, boost::shared_ptr<Variable_modifier> > ilang_Variable_modifier_list;


  class Variable {
  private:
    friend class ilang::Modification;
    std::list<shared_ptr<Variable_modifier> > Modifiers;
    std::string Name;
    bool Check (boost::any&);
    ValuePass val;
    Variable()=delete;
  public:
    Variable (std::string name, std::list<std::string> modifiers);
    void Set(ValuePass v);
    ValuePass Get();
    bool isSet();
    //~Variable();
  };

  // a wrapper class for any numerical value
  /*
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
    };*/

  class Value {
  private:
    boost::any val;
    friend class Variable;
  public:
    Value (boost::any);
    Value ();
    ~Value();
    void Print();
    std::string str();
    bool isTrue();
    boost::any & Get();
  };


}

#define ILANG_VARIABLE_MODIFIER(name, obj)				\
  namespace { struct _ILANG_VAR_MOD_##name {				\
  _ILANG_VAR_MOD_##name () {						\
    ::ilang::ilang_Variable_modifier_list[ #name ] =			\
      ::boost::shared_ptr< ::ilang::Variable_modifier > ( new obj );	\
  }} _ILANG_VAR_MOD_##name##_run;					\
  }


#endif // _ilang_variable
