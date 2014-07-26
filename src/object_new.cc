#include "object_new.h"
#include "function.h"
#include "error.h"
#include "value_types.h"

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
		auto var = make_handle<Variable>();
		var->Set(v);
		m_members.insert(pair<Identifier, Handle<Variable> >(i, var));
	} else {
		it->second->Set(v);
	}
}

bool Object_ish::has(Identifier i) {
	return m_members.find(i) != m_members.end();
}

Handle<Variable> Object_ish::getVariable(ilang::Identifier i) {
	if(i == "this") {
		// can just create a new variable here which holds the reference to
		// the this ptr
		auto ptr = Handle<Hashable>(this);
		auto ret = make_handle<Variable>();
		ret->Set(valueMaker(ptr));
		return ret;
	}
	auto it = m_members.find(i);
	assert(it != m_members.end());
	return it->second;
}


Class::Class() {

}

// Class_new::Class_new(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, ScopePass scope) {
// 	assert(p && obj && scope);
// 	for(auto it : *p) {

// 	}
// }

ValuePass Class::get(Identifier i) {
	if(has(i)) {
		return Object_ish::get(i);
	}
	// for(auto it : m_parents) {
	// 	if(it->has(i)) {
	// 		return
	// }
	if(i == Identifier("new")) {
		// new function
		// Calls create to create a new instance of the class,
		// and then calls init to set up the class with the passed arguments
		ValuePass create_fun = get(Identifier("create"));
		ilang::Function n([create_fun](Context &ctx, ilang::Arguments &args, ValuePass *ret) {
				ilang::Arguments na;
				*ret = create_fun->call(na);
				auto init = (*ret)->get(Identifier("init"));
				ValuePass ir = init->call(args);
			});
		return valueMaker(n);
	}
	if(i == Identifier("create")) {
		// creates a new instance of the class
		// but does not init it
		return ValuePass();
	}
	if(i == Identifier("init")) {
		ilang::Function fun;
		return valueMaker(fun);
	}
	if(i == Identifier("instance")) {
		// instance function
		return ValuePass();
	}
	if(i == Identifier("interface")) {
		// interface function
		return ValuePass();
	}
	assert(0); // so, not found
}

Object::Object() {

}

Object::Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Context &ctx) : Object_ish() {
	Context ctx_to;
	ctx_to.scope = this;
	for(auto it : *obj) {
		error(!has(Identifier(it.first->GetFirstName())), "setting variable twice in object");
		assert(dynamic_cast<ilang::parserNode::Value*>(it.second));
		ValuePass val = dynamic_cast<ilang::parserNode::Value*>(it.second)->GetValue(ctx);
		it.first->Set(ctx_to, val);
	}
}
