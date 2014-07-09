#include "object_new.h"
#include "function.h"

using namespace ilang;
using namespace std;

ValuePass Object_ish::get(Identifier i) {
	auto it = m_members.find(i);
	assert(it != m_members.end());
	ValuePass ret = it->second->Get();
	if(ret->type() == typeid(ilang::Function)) {
		auto ptr = m_self.lock();
		return valueMaker(ret->cast<ilang::Function*>()->bind(ptr.get()));
	}
	return ret;
}

void Object_ish::set(Identifier i, ValuePass v) {
	auto it = m_members.find(i);
	if(it == m_members.end()) {
		auto var = make_shared<Variable>();
		var->Set(v);
		m_members.insert(pair<Identifier, shared_ptr<Variable> >(i, var));
	} else {
		it->second->Set(v);
	}
}

bool Object_ish::has(Identifier i) {
	return m_members.find(i) != m_members.end();
}

shared_ptr<Variable> Object_ish::getVariable(ilang::Identifier i) {
	if(i == "this") {
		// can just create a new variable here which holds the reference to
		// the this ptr
		auto ptr = m_self.lock();
		auto ret = make_shared<Variable>();
		ret->Set(valueMaker(ptr));
		return ret;
	}
	auto it = m_members.find(i);
	assert(it != m_members.end());
	return it->second;
}




Class_new::Class_new() {

}

Class_new::Class_new(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, ScopePass scope) {
	assert(p && obj && scope);
	for(auto it : *p) {

	}

}
