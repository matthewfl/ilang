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

shared_ptr<ilang::Variable> Scope::forceNew(ilang::Identifier i, std::vector<ValuePass> modifiers) {
	assert(m_vars.find(i) == m_vars.end());
	for(auto it : modifiers) { assert(it); }
	auto var = make_shared<ilang::Variable>(modifiers);
	m_vars.insert(pair<Identifier, shared_ptr<Variable> >(i, var));
	return var;
}

ValuePass Scope::get(ilang::Identifier i) {
	cout << "Getting " << i.str() << endl;
	ValuePass ret;
	auto it = m_vars.find(i);
	if(it != m_vars.end()) {
		ret = it->second->Get();
	} else {
		assert(m_parent);
		ret = m_parent->get(i);
	}
	assert(ret);
	return ret;
}

void Scope::set(ilang::Identifier i, ValuePass v) {
	// TODO: optimize this??
	cout << "Setting " << i.str() << endl;
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

shared_ptr<Variable> Scope::getVariable(ilang::Identifier i) {
	auto it = m_vars.find(i);
	if(it == m_vars.end())
		return shared_ptr<Variable>();
	return it->second;
}

void Scope::insert(ilang::Identifier i, shared_ptr<Variable> v) {
	m_vars.insert(pair<Identifier, shared_ptr<Variable> >(i, v));
}


void Scope::Debug() {
	for(auto it : m_vars) {
		cout << it.first.str() << endl;//" " << it.second->Get()->cast<string>() << endl;
	}
}

static std::map<ilang::Identifier, ilang::ValuePass> *global_scope = NULL;

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
