#include "scope.h"


namespace scope {
  virtual ilang::Variable * Scope::_lookup (string &name) {
    ilang::Variable *v = vars->find(name);
    if(v) return v;
    if(parent) parent->_lookup(name);
    return NULL;
  }

  ilang::Variable * Scope::lookup (string name) {
    ilang::Variable * f = _lookup(name);
    if(f) return f;
    f = new ilang::Value;
    vars.insert(pair<string, ilang::Variable*>(name, f));
    return f;
  }
}
