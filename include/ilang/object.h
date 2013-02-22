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
    Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, ScopePass);
    virtual Object* NewClass(ValuePass self);
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
    ValuePass baseClassValue; // kept so the smart pointer to know that something is using it
    Class *baseClass;
    C_Class *C_baseClass;
    std::map<std::string, ilang::Variable*> members;
    void Debug();
    char *DB_name;
  public:
    Object(Class*, ValuePass);
    Object(C_Class *base);
    Object();
    virtual ~Object();
    Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*>*, ScopePass);
    virtual ilang::Variable * operator [] (std::string name);
    virtual ilang::Variable * operator [] (ValuePass);
  };
  class Array : public Object {
    friend storedData *DB_createStoredData(const boost::any&);
    friend ValuePass DB_readStoredData(storedData*);
    friend class Value;
    std::vector<ilang::Variable*> members;
    std::list<std::string> *modifiers;
    ilang::Variable *mem_length;
    ilang::Variable *mem_push;
    ilang::Variable *mem_pop;
    ilang::Variable *mem_insert;
    ilang::Variable *mem_remove;
    void Init();
    char *DB_name;
  public:
    Array(std::list<ilang::parserNode::Node*>*, std::list<std::string>*, ScopePass);
    Array(std::vector<ValuePass> &);
    Array();
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
