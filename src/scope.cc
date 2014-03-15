#include "scope.h"
#include "debug.h"

#include "object.h"


namespace ilang {
	using namespace std;
	ilang::Variable * Scope::_lookup (string &name) {
		auto it = vars.find(name);
		if(it != vars.end())
			return it->second;
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
		assert(vars.find(name) == vars.end());
		ilang::Variable *v = new ilang::Variable(name, modifiers);
		vars.insert(pair<string, ilang::Variable*>(name, v));
		return v;
	}

	int Scope::Debug() {
		return 0;
		debug_break(return 0;)
			int indent=1;
		if(parent) indent = parent->Debug();
		for(pair<const string, ilang::Variable*> i : vars) {
			for(int i=0;i<indent;++i) cout << "\t";
			cout << i.first << "\t" << i.second->Get() << endl;
		}

		return indent+1;
	}


	Scope::Scope(ScopePass p): parent(p) {}
	Scope::~Scope() {
		for(auto it : vars) {
			delete it.second;
		}
	}

	ilang::FileScope * Scope::fileScope() {
		if(parent) return parent->fileScope();
		assert(dynamic_cast<FileScope*>(this));
		return dynamic_cast<FileScope*>(this);
	}

	ilang::Variable * FileScope::_lookup (string &name) {
		//TODO: make this check to see if it can't find it
		return vars.find(name)->second; // there is nothing higher that can be looked at
	}

	ObjectScope::ObjectScope (Object *o) : Scope(ScopePass()), obj(o) {}
	ilang::Variable * ObjectScope::_lookup(std::string &name) {
		auto it = obj->members.find(name);
		if(it == obj->members.end()) {
			ilang::Variable *var;
			if(obj->baseClass) {
				var = obj->baseClass->operator[](name);
				if(var) return var;
			}
			return NULL;
		}
		return (it->second);
	}

}
