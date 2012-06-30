#ifndef _ilang_object
#define _ilang_object

#include <string>
#include <vector>
#include <map>
#include "variable.h"
#include "parserTree.h"
#include "scope.h"

namespace ilang {
  class Object;
  class Class {
  private:
    std::vector<Class*> parents;
    std::map<std::string, ilang::Variable> members;
  public:
    Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj);
    Object* NewClass();
    ilang::Variable * operator[](std::string name);
    ilang::Variable * operator[](ValuePass);
  };
  class Object {
  private:
    Class *baseClass;
    std::map<std::string, ilang::Variable> members;
    void Debug();
  public:
    Object(Class *base);
    Object();
    Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*>*, Scope*);
    virtual ilang::Variable * operator [] (std::string name);
    virtual ilang::Variable * operator [] (ValuePass);
  };
  class Array : public Object {
    std::vector<ilang::Variable> members;
  public:
    Array(std::list<ilang::parserNode::Node*>, Scope*);
    ilang::Variable * operator[] (std::string name);
    ilang::Variable * operator[] (ValuePass);
  };
}

#endif // _ilang_object
