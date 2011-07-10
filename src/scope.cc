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

  ilang::Variable * Scope::lookup (string name, list<string> modifiers) {
    ilang::Variable * f = _lookup(name);
    if(f) return f;
    f = new ilang::Variable(name, modifiers);
    vars.insert(pair<string, ilang::Variable*>(name, f));
    return f;
  }
  Scope::Scope(Scope *p): parent(p) {}

  FileScope::FileScope(): Scope((Scope *)NULL) {}
  
  ilang::Variable * FileScope::_lookup (string &name) {
    return vars.find(name)->second; // there is nothing higher that can be looked at
  }
}
