#include "variable_new.h"
#include "function.h"

using namespace ilang;

void Variable::Set(ValuePass v) {
	assert(v);
	for(auto it : m_modifiers) {
		assert(it);
		Arguments args(v);
		if(it->type() == typeid(Function)) {
			it->call(args);
		}else if(it->type() == typeid(Object*)) {
			Identifier check("check");
			auto c = it->get(check);
			c->call(args);
		}else{
			assert(0);
		}
	}
	m_value = v;
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
