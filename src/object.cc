#include "object.h"
#include "debug.h"

#include <iostream>
using namespace std;

namespace ilang {
  Class::Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj) {
  
  }
  Object* Class::NewClass() {
    return new Object(this);
  }

  Object::Object(Class *base): baseClass(base) {
    // I am not sure what this should look like, if it should check the base classes when accessing stuff or just copy it into a new object when one is created, if a new object is just copied then it would make it hard for the code modification to work
  }
  Object::Object(): baseClass(NULL) {}
  Object::Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Scope *scope): baseClass(NULL) {
    assert(obj);
    assert(scope);
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
  
  ilang::Variable * Object::operator [] (std::string name) {
    debug_num(5, Debug() );
    auto iter = members.find(name);
    if(iter == members.end()) {
      debug(4, "Member "<< name << " not found in object");
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
  
}
