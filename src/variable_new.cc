#include "variable_new.h"
#include "function.h"
#include "exception.h"

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
	for(auto it : m_modifiers) {
		assert(it);
		Arguments args(v);
		ValuePass ret;
		if(it->type() == typeid(Function)) {
			ret = it->call(args);
		}else if(it->type() == typeid(Object*)) {
			Identifier check("check");
			auto c = it->get(check);
			if (c) ret = c->call(args);
		}else{
			assert(0);
		}
		if(ret && ret->cast<bool>() == false) {
			throw BadTypeCheck();
		}
	}
}

Variable::Variable(std::vector<ilang::ValuePass> mod) {
	m_modifiers.reserve(mod.size());
	for(auto it : mod) {
		if(it->type() == typeid(ilang::Class*)) {
			// create a new instance of the class
			assert(0);
		} else {
			m_modifiers.push_back(it);
		}
	}
}
