#ifndef _ilang_scope
#define _ilang_scope

#include <map>
#include <string>
#include "variable.h"


namespace ilang {
  class FileScope;
  class Scope {
  protected:
    std::map<std::string, ilang::Variable*> vars;
    Scope *parent;
    virtual ilang::Variable * _lookup (std::string &name);
  public:
    ilang::Variable * lookup (std::string name);
    ilang::Variable * forceNew (std::string name, std::list<std::string> &modifiers);
    ilang::FileScope * fileScope ();
    virtual void ParentReturn(ValuePass *val) { assert(parent); parent->ParentReturn(val); }
    Scope(Scope *parent);
    virtual ~Scope();

    int Debug();    
  };
  class FileScope : public Scope {
  protected:
    virtual ilang::Variable * _lookup (std::string &name);
  public:
    FileScope(): Scope((Scope *)NULL) {}
  };
  template <typename ReturnHook> class FunctionScope : public Scope {
  public:
    FunctionScope(Scope *s, ReturnHook h) : Scope(s), hook(h) {}
    void ParentReturn(ValuePass *r) { hook(r); }
  private:
    ReturnHook hook;
  };
  class ClassScope : public Scope {
  public:
    ClassScope(Scope *s) : Scope(s) {}
  };
  
}; // namespace ilang

#endif // _ilang_scope
