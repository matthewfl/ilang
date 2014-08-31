#include "variable_new.h"
#include "function.h"
#include "exception.h"
#include "value_types.h"

using namespace ilang;

void Variable::Set(ValuePass v) {
	assert(v);
	Check(v);
	Arguments args(v);
	for(auto it : m_modifiers) {
		if(it->type() == typeid(Object*)) {
			Identifier set("setting");
			auto c = it->get(set);
			if(c) c->call(args);
		}
	}
	m_value = v;
}

void Variable::Check(ValuePass v) {
	Identifier check("check");
	for(auto it : m_modifiers) {
		assert(it);
		Arguments args(v);
		ValuePass ret;
		if(it->type() == typeid(Function)) {
			ret = it->call(args);
		}else if(it->type() == typeid(Hashable*)) {
			auto c = it->get(check);
			if (c) ret = c->call(args);
		}else{
			assert(0);
		}
		if(ret && ret->cast<bool>() == false) {
			throw BadTypeCheck();
		}
	}
	Identifier setting("setting");
	Arguments args(v);
	for(auto it : m_modifiers) {
		if(it->type() == typeid(Hashable*)) {
			auto s = it->get(setting);
			if(s) s->call(args);
		}
	}
}

ValuePass Variable::Get() {
	ValuePass ret;
	Identifier getting("getting");
	Arguments na;
	for(auto it : m_modifiers) {
		if(it->type() == typeid(Hashable*)) {
			auto g = it->get(getting);
			if(g) ret = g->call(na);
			if(ret) return ret;
		}
	}
	assert(m_value);
	return m_value;
}

void Variable::SetModifiers(std::vector<ilang::ValuePass> mod) {
	assert(m_modifiers.empty());
	m_modifiers.reserve(mod.size());
	for(auto it : mod) {
		if(it->type() == typeid(ilang::Hashable*)) {
			auto n = it->get(Identifier("new"));
			if(n) {
				Arguments na;
				m_modifiers.push_back(n->call(na));
			} else {
				m_modifiers.push_back(it);
			}
			// create a new instance of the class
			//assert(0);
		} else {
			m_modifiers.push_back(it);
		}
	}
}

Variable::Variable(std::vector<ilang::ValuePass> mod) {
	SetModifiers(mod);
}

Variable::Variable(const Variable &v) {
	m_modifiers = v.m_modifiers;
	m_value = v.m_value;
}


void BoundVariable::Set(ValuePass v) {
	m_parent->Set(v);
}

void BoundVariable::Check(ValuePass v) {
	m_parent->Check(v);
}

void BoundVariable::SetModifiers(std::vector<ValuePass> vec) {
	m_parent->SetModifiers(vec);
}

ValuePass BoundVariable::Get() {
	ValuePass ret = m_parent->Get();
	if(ret->type() == typeid(Function)) {
		return valueMaker(ret->cast<Function*>()->bind(m_bound));
	}
	return ret;
}
