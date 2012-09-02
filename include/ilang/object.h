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
    std::map<std::string, ilang::Variable*> members;
  public:
    Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Scope*);
    //virtual Object* NewClass();
    ilang::Variable * operator[](std::string name);
    ilang::Variable * operator[](ValuePass);
    virtual ~Class();
  };
  class ObjectScope;
  struct storedData;
  class C_Class;
  class Object {
  private:
    friend class ObjectScope;
    friend storedData *DB_createStoredData(const boost::any&);
    friend ValuePass DB_readStoredData(storedData*);
    ValuePass baseClassValue;
    Class *baseClass;
    C_Class *C_baseClass;
    std::map<std::string, ilang::Variable*> members;
    void Debug();
    char *DB_name;
  public:
    Object(ValuePass);
    Object(C_Class *base);
    Object();
    virtual ~Object();
    Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*>*, Scope*);
    virtual ilang::Variable * operator [] (std::string name);
    virtual ilang::Variable * operator [] (ValuePass);
  };
  class Array : public Object {
    std::vector<ilang::Variable*> members;
    std::list<std::string> *modifiers;
  public:
    Array(std::list<ilang::parserNode::Node*>*, std::list<std::string>*, Scope*);
    ilang::Variable * operator[] (std::string name);
    ilang::Variable * operator[] (ValuePass);
    virtual ~Array();
  };

  class ScopeObject : public Object {
  private:
    Scope *scope;
    bool Isolate;
  public:
    // isolate causes variables that are modified to be put into their own copy of the object rather than the scope
    ScopeObject(Scope*, bool isolate=true);
    ilang::Variable * operator[] (std::string name);
    ilang::Variable * operator[] (ValuePass);
  };
}

#endif // _ilang_object
