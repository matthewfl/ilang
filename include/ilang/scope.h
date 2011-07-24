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
    Scope(Scope *parent);
  };
  class FileScope : public Scope {
  protected:
    virtual ilang::Variable * _lookup (std::string &name);
  public:
    FileScope(): Scope((Scope *)NULL) {}
  };
  class FunctionScope : public Scope {
  public:
    FunctionScope(Scope *s) : Scope(s) {}
  };
  class ClassScope : public Scope {
  public:
    ClassScope(Scope *s) : Scope(s) {}
  };
  
}; // namespace ilang

#endif // _ilang_scope
