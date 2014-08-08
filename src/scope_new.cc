#include "scope_new.h"

using namespace ilang;
using namespace std;

Scope::~Scope() {
	assert(m_ctx->scope == this);
	m_ctx->scope = m_parent;
}

// ilang::Variable * Scope::_lookup(ilang::Identifier i) {
// 	auto it = m_vars.find(i);
// 	if(it != m_vars.end())
// 		return &m_vars.second;
// 	//if(parent)
// 	//return parent->_lookup(i);
// 	return NULL;
// }

Handle<Variable> Scope::forceNew(ilang::Identifier i, std::vector<ValuePass> modifiers) {
	assert(m_vars.find(i) == m_vars.end());
	for(auto it : modifiers) { assert(it); }
	auto var = make_handle<ilang::Variable>(modifiers);
	m_vars.insert(pair<Identifier, Handle<Variable> >(i, var));
	return var;
}

ValuePass Scope::get(ilang::Identifier i) {
	//cout << "Getting " << i.str() << endl;
	ValuePass ret;
	auto it = m_vars.find(i);
	if(it != m_vars.end()) {
		ret = it->second->Get();
	} else {
		if(!m_parent) {
			// just use the global scope if there is no parent
			ret = global_scope_lookup(i);
		} else
			ret = m_parent->get(i);
	}
	assert(ret);
	return ret;
}

void Scope::set(ilang::Identifier i, ValuePass v) {
	// TODO: optimize this??
	//cout << "Setting " << i.str() << endl;
	assert(v);
	if(m_parent && m_parent->has(i)) {
		m_parent->set(i, v);
		return;
	}
	auto it = m_vars.find(i);
	if(it != m_vars.end()) {
		it->second->Set(v);
		return;
	}
	auto var = forceNew(i, {});
	var->Set(v);
}

bool Scope::has(ilang::Identifier i) {
	return m_vars.find(i) != m_vars.end() || m_parent && m_parent->has(i);
}

Handle<Variable> Scope::getVariable(ilang::Identifier i) {
	// TODO: this needs to call parent scopes, so that bind can bind to higher scopes..
	auto it = m_vars.find(i);
	if(it == m_vars.end()) {
		if(m_parent) {
			return m_parent->getVariable(i);
		} else {
			ValuePass g = global_scope_lookup(i);
			if(g) {
				auto var = make_handle<Variable>();
				var->Set(g);
				return var;
			}
			return Handle<Variable>();
		}
	}
	return it->second;
}

void Scope::insert(ilang::Identifier i, Handle<Variable> v) {
	m_vars.insert(pair<Identifier, Handle<Variable> >(i, v));
}

Hashable_iterator Scope::begin() {
	assert(!m_parent);
	return Hashable_iterator(m_vars.begin());
}

Hashable_iterator Scope::end() {
	assert(!m_parent);
	return Hashable_iterator(m_vars.end());
}


void Scope::Debug() {
	for(auto it : m_vars) {
		cout << it.first.str() << endl;//" " << it.second->Get()->cast<string>() << endl;
	}
}

static std::map<ilang::Identifier, ilang::ValuePass> *global_scope = NULL;

namespace ilang {

	ValuePass global_scope_lookup(ilang::Identifier i) {
		if(!global_scope)
			global_scope = new std::map<ilang::Identifier, ilang::ValuePass>();
		auto it = global_scope->find(i);
		if(it == global_scope->end())
			return ValuePass();
		return it->second;
	}

	void global_scope_register(ilang::Identifier i, ValuePass v) {
		if(!global_scope)
			global_scope = new std::map<ilang::Identifier, ilang::ValuePass>();
		global_scope->insert(std::pair<ilang::Identifier, ilang::ValuePass>(i, v));
	}

}
