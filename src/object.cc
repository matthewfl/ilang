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
  Object::Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Scope *scope): baseClass(NULL) {
    for(auto it : *obj) {
      std::string name = it.first->name->front();
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
      debug(0, "Member "<< name << " not found in object");
      // I guess create a new one and insert it
      members.insert(pair<std::string, ilang::Variable>(name, Variable(name, list<string>())));
      return &(members.find(name)->second);
      //assert(0);
    }
    return &(iter->second);
  }

  void Object::Debug() {
    for(auto it : members) {
      auto s = it.second.Get();
      cout << "\t\t" << it.first << "\t" << s->Get().type().name() << "\t" << endl;
      s->Print();
    }
  }
  
  ilang::Variable * Array::operator [] (long place) {
  }

  ilang::Variable * Array::operator[] (std::string name) {
    if(name == "length") {
      ilang::Value *val = new ilang::Value((long)members.size());
      
    }
    // idk what else this can be/should be
    // maybe return functions such as push and pop etc
    assert(0);
    
  }
  
}
