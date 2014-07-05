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
	auto var = make_shared<ilang::Variable>(modifiers);
	m_vars.insert(pair<Identifier, shared_ptr<Variable> >(i, var));
	return var;
}

ValuePass Scope::get(ilang::Identifier i) {
	auto it = m_vars.find(i);
	if(it != m_vars.end())
		return it->second->Get();
	assert(m_parent);
	return m_parent->get(i);
}

void Scope::set(ilang::Identifier i, ValuePass v) {
	// TODO: optimize this??
	if(m_parent && m_parent->has(i))
		m_parent->set(i, v);
	else
		forceNew(i, {})->Set(v);
}

bool Scope::has(ilang::Identifier i) {
	return m_vars.find(i) != m_vars.end() || m_parent->has(i);
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
