#include "scope.h"

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

Handle<Variable> Scope::forceNew(Context &ctx, ilang::Identifier i, std::vector<ValuePass> modifiers) {
	//assert(m_vars.find(i) == m_vars.end());
	for(auto it : modifiers) { assert(it); }
	Handle<Variable> var;
	auto it = m_vars.find(i);
	if(it != m_vars.end()) {
		var = it->second;
		var->SetModifiers(ctx, modifiers);
	} else {
		var = make_handle<ilang::Variable>(ctx, modifiers);
		m_vars.insert(pair<Identifier, Handle<Variable> >(i, var));
	}
	return var;
}

ValuePass Scope::get(Context &ctx, ilang::Identifier i) {
	//cout << "Getting " << i.str() << endl;
	ValuePass ret;
	auto it = m_vars.find(i);
	if(it != m_vars.end()) {
		ret = it->second->Get(ctx);
	} else {
		if(!m_parent) {
			// just use the global scope if there is no parent
			ret = global_scope_lookup(i);
		} else
			ret = m_parent->get(ctx, i);
	}
	assert(ret); // TODO: remove, just return an empty object
	return ret;
}

void Scope::set(Context &ctx, ilang::Identifier i, ValuePass v) {
	// TODO: optimize this??
	//cout << "Setting " << i.str() << endl;
	assert(v);
	if(m_parent && m_parent->has(ctx, i)) {
		m_parent->set(ctx, i, v);
		return;
	}
	auto it = m_vars.find(i);
	if(it != m_vars.end()) {
		it->second->Set(ctx, v);
		return;
	}
	auto var = forceNew(ctx, i, {});
	var->Set(ctx, v);
}

bool Scope::has(Context &ctx, ilang::Identifier i) {
	return m_vars.find(i) != m_vars.end() || m_parent && m_parent->has(ctx, i);
}

Handle<Variable> Scope::getVariable(Context &ctx, ilang::Identifier i) {
	// TODO: this needs to call parent scopes, so that bind can bind to higher scopes..
	auto it = m_vars.find(i);
	if(it == m_vars.end()) {
		if(m_parent) {
			return m_parent->getVariable(ctx, i);
		} else {
			ValuePass g = global_scope_lookup(i);
			if(g) {
				auto var = make_handle<Variable>();
				var->Set(ctx, g);
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
