#include "object.h"
#include "debug.h"

// I do not like including this one here, but it is for C_Class->operator[]
#include "ilang.h"

#include <iostream>
using namespace std;

namespace ilang {
  Class::Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Scope* scope) {
    if(!p && !obj && !scope) return;
    assert(p);
    assert(obj);
    assert(scope);
    
    for(auto it : *p) {
      assert(dynamic_cast<ilang::parserNode::Value*>(it));
      boost::any & a = dynamic_cast<ilang::parserNode::Value*>(it)->GetValue(scope)->Get();
      // this will probably need to get changed and stuff, idk
      // this is going to return what ever type is returned by the GetValue from class
      assert(a.type() == typeid(Class*));
      parents.push_back(boost::any_cast<Class*>(a));
    }
    for(auto it : *obj) {
      std::string name = it.first->GetFirstName();
      if(members.find(name) != members.end()) {
	debug(0, "Member in class overwritten within self");
	assert(0);
      }
      ilang::Variable var(name, *(it.first->modifiers));
      if(it.second) {
	assert(dynamic_cast<ilang::parserNode::Value*>(it.second));
	var.Set(dynamic_cast<ilang::parserNode::Value*>(it.second)->GetValue(scope));
      }
      members.insert(pair<std::string, ilang::Variable>(name, var));
    }
  }
  Object* Class::NewClass() {
    return new Object(this);
  }
  ilang::Variable * Class::operator[](std::string name) {
    ilang::Variable *var;
    auto search = members.find(name);
    if(search == members.end()) {
      for(auto it : parents) {
	var = it->operator[](name);
	if(var) return var;
      }
    }else{
      return &(search->second);
    }
    return NULL;
  }
  ilang::Variable * Class::operator[](ValuePass val) {
    if(val->Get().type() == typeid(std::string))
      return operator[](boost::any_cast<std::string>(val->Get()));
    assert(0); 
    return NULL; // this most likely will get changed in the future 
  }

  Object::Object(Class *base): baseClass(base), C_baseClass(NULL) {
    // I am not sure what this should look like, if it should check the base classes when accessing stuff or just copy it into a new object when one is created, if a new object is just copied then it would make it hard for the code modification to work
    std::list<std::string> this_mod = {"Const"};
    ilang::Variable this_var("this", this_mod);
    this_var.Set(ValuePass(new ilang::Value(this)));
    members.insert(pair<std::string, ilang::Variable>("this", this_var)); 
  }
  Object::Object(C_Class *base): C_baseClass(base), baseClass(NULL) {}
  Object::Object(): baseClass(NULL), C_baseClass(NULL) {
    std::list<std::string> this_mod = {"Const"};
    ilang::Variable this_var("this", this_mod);
    this_var.Set(ValuePass(new ilang::Value(this)));
    members.insert(pair<std::string, ilang::Variable>("this", this_var)); 
  }
  Object::Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Scope *scope): baseClass(NULL), C_baseClass(NULL) {
    assert(obj);
    assert(scope);
    std::list<std::string> this_mod = {"Const"};
    ilang::Variable this_var("this", this_mod);
    this_var.Set(ValuePass(new ilang::Value(this)));
    members.insert(pair<std::string, ilang::Variable>("this", this_var)); 
    for(auto it : *obj) {
      //assert(it.first->name);
      std::string name = it.first->GetFirstName();
      if(members.find(name) != members.end()) {
	debug(0, "Member in Object over written");
	assert(0);
      }
      ilang::Variable var(name, *(it.first->modifiers));
      if(it.second) { // != NULL
	assert(dynamic_cast<ilang::parserNode::Value*>(it.second));
	var.Set(dynamic_cast<ilang::parserNode::Value*>(it.second)->GetValue(scope));
      }
      members.insert(pair<std::string, ilang::Variable>(name, var));
    }
  }

  Object::~Object () {
    if(C_baseClass) delete C_baseClass;
  }
  
  ilang::Variable * Object::operator [] (std::string name) {
    debug_num(5, Debug() );
    auto iter = members.find(name);
    if(iter == members.end()) {
      if(baseClass) {
	// need to make something if the variable is getting set so that it is save differently
	ilang::Variable * var = baseClass->operator[](name);
	if(var)
	  return var;
      }else if(C_baseClass) {
	ilang::Variable *var = C_baseClass->operator[](name);
	if(var)
	  return var;
      } 
      // if we don't find the variable then make a new one and return it
      debug(0, "Member "<< name << " not found in object");
      // I guess create a new one and insert it
      members.insert(pair<std::string, ilang::Variable>(name, Variable(name, list<string>())));
      return &(members.find(name)->second);
      //assert(0);
      
    }
    
    return &(iter->second);
  }

  ilang::Variable * Object::operator[] (ValuePass name) {
    // the interface should be changed so that any value can be looked up in an object
    boost::any & n = name->Get();
    assert(n.type() == typeid(std::string));
    return operator[](boost::any_cast<std::string>(n));
  }

  void Object::Debug() {
    for(auto it : members) {
      auto s = it.second.Get();
      cout << "\t\t" << it.first << "\t" << s->Get().type().name() << "\t" << endl;
      s->Print();
    }
  }
  
  Array::Array (std::list<ilang::parserNode::Node*> *elements, std::list<std::string> *mod, Scope *scope) :modifiers(mod) {
    members.reserve(elements->size());
    for(auto it=elements->begin(); it!= elements->end(); it++) {
      ilang::parserNode::Value *v = dynamic_cast<parserNode::Value*>(*it);
      assert(v);
      ilang::Variable var("", *modifiers);
      var.Set(v->GetValue(scope));
      members.push_back(var);
    }
  }
  
  ilang::Variable * Array::operator [] (ValuePass name) {
    boost::any & n = name->Get();
    if(n.type() == typeid(long)) { // look for the array number
      long place = boost::any_cast<long>(n);
      return &(members[place]);
    }else if(n.type() == typeid(std::string)) {
      return operator[](boost::any_cast<std::string>(n));
    }
  }

  // this most likely will leak memory big time until there is some real memory management
  ilang::Variable * Array::operator[] (std::string name) {
    if(name == "length") {
      ilang::Value *val = new ilang::Value((long)members.size());
      list<string> bm;
      ilang::Variable * v = new ilang::Variable("length", bm);
      v->Set(ValuePass(val));
      return v; // this is going to end up leaking, needs to get fixed etc
    }
    // idk what else this can be/should be
    // maybe return functions such as push and pop etc
    assert(0);
    
  }

  ScopeObject::ScopeObject (Scope *_scope, bool isolate) : scope(_scope), Isolate(isolate) {}
  ilang::Variable * ScopeObject::operator [] (std::string name) {
    //
    return scope->lookup(name);
  }
  ilang::Variable * ScopeObject::operator[] (ValuePass val) {
    boost::any &a = val->Get();
    assert(a.type() == typeid(std::string));
    return operator[](boost::any_cast<std::string>(a));
  }
}


namespace {
  class Class_var_type : public ilang::Variable_modifier {
  public:
    bool Check (const boost::any &val) {
      return val.type() == typeid(ilang::Class);
    }
  };
  ILANG_VARIABLE_MODIFIER(Class, Class_var_type)

  class Object_var_type : public ilang::Variable_modifier {
    bool Check (const boost::any &val) {
      return val.type() == typeid(ilang::Object);
    }
  };

  ILANG_VARIABLE_MODIFIER(Object, Object_var_type)
}
