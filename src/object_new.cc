#include "object_new.h"
#include "function.h"
#include "error.h"
#include "value_types.h"
#include "exception.h"
#include "parserTree.h"

using namespace ilang;
using namespace std;

ValuePass Object_ish::get(Identifier i) {
	// auto it = m_members.find(i);
	// assert(it != m_members.end());
	// ValuePass ret = it->second->Get();
	// if(ret->type() == typeid(ilang::Function)) {
	// 	return valueMaker(ret->cast<ilang::Function*>()->bind(this));
	// }
	// return ret;

	auto var = getVariable(i);
	assert(var);
	return var->Get();

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

Hashable_iterator Object_ish::begin() {
	return Hashable_iterator(m_members.begin());
}

Hashable_iterator Object_ish::end() {
	return Hashable_iterator(m_members.end());
}


Class::Class() {

}

Class::Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Context &ctx) {
	assert(p && obj);
	m_parents.reserve(p->size());
	Context self;
	self.scope = this;
	for(auto it : *p) {
		auto v = dynamic_cast<parserNode::Value*>(it)->GetValue(ctx);
		auto ptr = v->cast<Class*>();
		error(ptr, "Class can not inherit from non class");
		m_parents.push_back(v);
		m_members.insert(ptr->begin(), ptr->end());
	}
	for(auto it : *obj) {
		// TODO: if there is no default value being set
		assert(it.second);
		it.first->Set(self, dynamic_cast<parserNode::Value*>(it.second)->GetValue(ctx), true);
	}
}

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
		Handle<Class> cls(this);
		ilang::Function crt([cls](Context &ctx, ilang::Arguments &args, ValuePass *ret) {
				auto c = make_handle<Class_instance>(cls);
				// for(auto it : args.kwargs) {
				// 	c->set(it.first, it.second);
				// }
				*ret = valueMaker(c);
			});
		return valueMaker(crt);
	}
	if(i == Identifier("init")) {
		ilang::Function fun([](Context &ctx, ilang::Arguments &args, ValuePass *ret) {});
		return valueMaker(fun);
	}
	if(i == Identifier("instance")) {
		// instance function
		Handle<Class> cls(this);
		ilang::Function inst([cls](Context &ctx, ilang::Arguments &args, ValuePass *ret) {
				Hashable *hash;
				args.inject(hash);
				auto c = dynamic_cast<Class_instance*>(hash);
				// TODO: check if one of the parent classes
				bool r = c && (c->m_class == cls);
				*ret = valueMaker(r);
			});
		return valueMaker(inst);
	}
	if(i == Identifier("interface")) {
		// interface function
		Handle<Class> cls(this);
		ilang::Function inter([cls](Context &ctx, ilang::Arguments &args, ValuePass *ret) {
				Hashable *hash;
				args.inject(hash);
				try {
					for(auto it : *cls) {
						ValuePass v = hash->get(it.first);
						if(!v) {
							*ret = valueMaker(false);
							return;
						}
						it.second->Check(v);
					}
				} catch(BadType &e) {
					*ret = valueMaker(false);
					return;
				}
				*ret = valueMaker(true);
			});
		return valueMaker(inter);
	}
	assert(0); // so, not found
}




Class_instance::Class_instance(Handle<Class> c) : m_class(c) {}
//Class_instance::Class_instance(C_class *c) : m_ccclas(c) {}

ValuePass Class_instance::get(Identifier i) {
	if(Object_ish::has(i))
		return Object_ish::get(i);
	assert(m_class);
	return m_class->get(i);
}

bool Class_instance::has(Identifier i) {
	return Object_ish::has(i) || m_class->has(i);
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

Array::Array(std::list<ilang::parserNode::Node*> *mods, std::list<ilang::parserNode::Node*> *elems, Context &ctx) {
	m_modifiers.reserve(mods->size());
	for(auto it : *mods) {
		m_modifiers.push_back(dynamic_cast<parserNode::Value*>(it)->GetValue(ctx));
	}
	m_members.reserve(elems->size());
	for(auto it : *elems) {
		auto var = make_handle<Variable>(m_modifiers);
		var->Set(dynamic_cast<parserNode::Value*>(it)->GetValue(ctx));
		m_members.push_back(var);
	}
}

ValuePass Array::get(Identifier i) {
	if(i.raw() < 0x0200000000000000) { // this is an item in the array
		if(m_members.size() < i.raw()) {
			assert(0); // TODO: raise an exception or something...
		}
		auto v = m_members.at(i.raw());
		auto val = v->Get();
		return val;
		//return v->Get();
	}
	if(i == Identifier("length")) {
		return valueMaker(m_members.size());
	}
	if(i == Identifier("push")) {
		Handle<Array> self(this);
		Function p([self](Context &ctx, Arguments &args, ValuePass *ret) {
				auto v = make_handle<Variable>(self->m_modifiers);
				v->Set(args[0]);
				self->m_members.push_back(v);
				*ret = valueMaker(self->m_members.size());
			});
		return valueMaker(p);
	}
	if(i == Identifier("pop")) {
		Handle<Array> self(this);
		Function p([self](Context &ctx, Arguments &args, ValuePass *ret) {
				auto v = self->m_members.back();
				self->m_members.pop_back();
				*ret = v->Get();
			});
		return valueMaker(p);
	}

	error(0, "Array does not have member " << i.str());
	// TODO: other array methods
}

void Array::set(Identifier i, ValuePass v) {
	if(i.raw() >= 0x0200000000000000) {
		assert(0); // trying to set some string type???
	}
	// TODO: if member already exists use that variable
	auto h = make_handle<Variable>(m_modifiers);
	h->Set(v);
	m_members[i.raw()] = h;
}

bool Array::has(Identifier i) {
	if(i.raw() < 0x0200000000000000) {
		return i.raw() < m_members.size();
	}
	return i == Identifier("length") ||
		i == Identifier("push") ||
		i == Identifier("pop");
}

Handle<Variable> Array::getVariable(Identifier i) {
	if(i.raw() >= 0x0200000000000000) {
		assert(0); // well there is basically no variable
	}
	if(i.raw() >= m_members.size()) {
		assert(0); // TODO: exception about not having...
	}
	auto h = m_members[i.raw()];
	return h;
}
