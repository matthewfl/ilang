#ifndef _ilang_scope
#define _ilang_scope

#include <map>
#include <string>
#include "variable.h"


namespace ilang {
  class scope {
  private:
    std::map<std::string, ilang::Value*> vars;
    scope *parent;
  public:
    ilang::Variable * lookup (std::string name);
  };
  
}; // namespace ilang

#endif // _ilang_scope
