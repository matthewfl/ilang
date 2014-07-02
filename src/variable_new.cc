#include "variable_new.h"
#include "function.h"

using namespace ilang;

void Variable::Set(ValuePass v) {
	for(auto it : m_modifiers) {
		ilang::Arguments args(v);
		if(it->type() == typeid(ilang::Function*)) {
			it->call(args);
		}else if(it->type() == typeid(ilang::Object*)) {
			Identifier check("check");
			auto c = it->get(check);
			ilang::Arguments args(c);
			c->call(args);
		}else{
			assert(0);
		}
	}
	m_value = v;
}
