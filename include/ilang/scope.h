#ifndef _ilang_scope
#define _ilang_scope

#include <map>
#include <string>
#include "variable.h"


namespace ilang {
  class Scope {
  protected:
    std::map<std::string, ilang::Variable*> vars;
    Scope *parent;
    virtual ilang::Variable * _lookup (std::string &name);
  public:
    ilang::Variable * lookup (std::string name, std::list<std::string> modifiers);
    ilang::Variable * forceNew (std::string name, std::list<std::string> modifiers);
    Scope(Scope *parent);
  };
  class FileScope : public Scope {
  protected:
    virtual ilang::Variable * _lookup (std::string &name);
  public:
    FileScope();
  };
  class FunctionScope : public Scope {
  };
  class ClassScope : public Scope {
    
  };
  
}; // namespace ilang

#endif // _ilang_scope
