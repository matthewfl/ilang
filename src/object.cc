#include "object.h"

namespace ilang {
  Class::Class(std::list<Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj) {
  
  }
  Object* Class::NewClass() {
    return new Object(this);
  }

  Object::Object(Class *base): baseClass(base) {}
  Object::Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj): baseClass(NULL) {
    for(auto it : 
  }
  
  ilang::Variable & Object::operator [] (std::string name) {
    //return objects
  }
  
}
