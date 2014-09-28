#include "variable_new.h"
#include "function.h"
#include "exception.h"
#include "value_types.h"

using namespace ilang;

void Variable::Set(Context &ctx, ValuePass v) {
	assert(v);
	Check(ctx, v);
	Arguments args(v);
	Identifier set("setting");
	for(auto it : m_modifiers) {
		if(it->type() == typeid(Hashable*)) {
			auto c = it->get(ctx, set);
			if(c) c->call(ctx, args);
		}
	}
	m_value = v;
}

void Variable::Check(Context &ctx, ValuePass v) {
	Identifier check("check");
	for(auto it : m_modifiers) {
		assert(it);
		Arguments args(v);
		ValuePass ret;
		if(it->type() == typeid(Function)) {
			ret = it->call(ctx, args);
		}else if(it->type() == typeid(Hashable*)) {
			auto c = it->get(ctx, check);
			if (c) ret = c->call(ctx, args);
		}else{
			assert(0);
		}
		if(ret && ret->cast<bool>() == false) {
			throw BadTypeCheck();
		}
	}
	// Identifier setting("setting");
	// Arguments args(v);
	// for(auto it : m_modifiers) {
	// 	if(it->type() == typeid(Hashable*)) {
	// 		auto s = it->get(setting);
	// 		if(s) s->call(ctx, args);
	// 	}
	// }
}

ValuePass Variable::Get(Context &ctx) {
	ValuePass ret;
	Identifier getting("getting");
	Arguments na;
	for(auto it : m_modifiers) {
		if(it->type() == typeid(Hashable*)) {
			auto g = it->get(ctx, getting);
			if(g) ret = g->call(ctx, na);
			if(ret) return ret;
		}
	}
	assert(m_value);
	return m_value;
}

void Variable::SetModifiers(Context &ctx, std::vector<ilang::ValuePass> mod) {
	assert(m_modifiers.empty());
	m_modifiers.reserve(mod.size());
	Identifier nident("new");
	for(auto it : mod) {
		if(it->type() == typeid(ilang::Hashable*)) {
			auto n = it->get(ctx, nident);
			if(n) {
				Arguments na;
				m_modifiers.push_back(n->call(ctx, na));
			} else {
				m_modifiers.push_back(it);
			}
			// create a new instance of the class
			//assert(0);
		} else if(it->type() == typeid(VariableType)) {
			// ignore, as atm only deals with the placement of the variables in the scopes
		}else {
			m_modifiers.push_back(it);
		}
	}
}

Variable::Variable(Context &ctx, std::vector<ilang::ValuePass> mod) {
	SetModifiers(ctx, mod);
}

Variable::Variable(const Variable &v) {
	m_modifiers = v.m_modifiers;
	m_value = v.m_value;
}


void BoundVariable::Set(Context &ctx, ValuePass v) {
	m_parent->Set(ctx, v);
}

void BoundVariable::Check(Context &ctx, ValuePass v) {
	m_parent->Check(ctx, v);
}

void BoundVariable::SetModifiers(Context &ctx, std::vector<ValuePass> vec) {
	m_parent->SetModifiers(ctx, vec);
}

ValuePass BoundVariable::Get(Context &ctx) {
	ValuePass ret = m_parent->Get(ctx);
	if(ret->type() == typeid(Function)) {
		return valueMaker(ret->cast<Function*>()->bind(m_bound));
	}
	return ret;
}
