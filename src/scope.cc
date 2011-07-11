#include "scope.h"
#include <assert.h>


namespace ilang {
  using namespace std;
  ilang::Variable * Scope::_lookup (string &name) {
    ilang::Variable *v = vars.find(name)->second; // I think that this will work, but the dereference might crash it
    if(v) return v;
    assert(parent);
    return parent->_lookup(name);
  }

  ilang::Variable * Scope::lookup (string name) {
    ilang::Variable * f = _lookup(name);
    if(f) return f;
    list<string> mod;
    f = new ilang::Variable(name, mod);
    vars.insert(pair<string, ilang::Variable*>(name, f));
    return f;
  }

  ilang::Variable * Scope::forceNew (string name, std::list<std::string> &modifiers) {
    ilang::Variable *v = new ilang::Variable(name, modifiers);
    vars.insert(pair<string, ilang::Variable*>(name, v));
    return v;
  }

  Scope::Scope(Scope *p): parent(p) {}

 
  
  ilang::Variable * FileScope::_lookup (string &name) {
    return vars.find(name)->second; // there is nothing higher that can be looked at
  }
}
