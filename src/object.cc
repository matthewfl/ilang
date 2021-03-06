#include "object.h"
#include "function.h"
#include "error.h"
#include "value_types.h"
#include "exception.h"
#include "parserTree.h"

//#include <iostream>

using namespace ilang;
using namespace std;

ValuePass Object_ish::get(Context &ctx, Identifier i) {
	auto var = getVariable(ctx, i);
	if(!var) {
		return ValuePass();
	}
	ValuePass ret = var->Get(ctx);
	if(ret->type() == typeid(Function))
		return valueMaker(ret->cast<Function*>()->rebind(this));
	return ret;

}

void Object_ish::set(Context &ctx, Identifier i, ValuePass v) {
	auto it = m_members.find(i);
	if(it == m_members.end()) {
		auto var = make_handle<Variable>();
		var->Set(ctx, v);
		m_members.insert(pair<Identifier, Handle<Variable> >(i, var));
	} else {
		it->second->Set(ctx, v);
	}
}

bool Object_ish::has(Context &ctx, Identifier i) {
	return m_members.find(i) != m_members.end();
}

Handle<Variable> Object_ish::getVariable(Context &ctx, ilang::Identifier i) {
	if(i == "this") {
		auto ptr = Handle<Hashable>(this);
		auto ret = make_handle<Variable>();
		ret->Set(ctx, valueMaker(ptr));
		return ret;
	}
	auto it = m_members.find(i);
	if(it == m_members.end())
		return NULL;
	auto bd = valueMaker(Handle<Hashable>(this));
	return make_handle<BoundVariable>(it->second, bd);
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
		// TODO: wrong
		// these are inheriting the variables from their parents, which means
		// the parents values can be set
		m_members.insert(ptr->begin(), ptr->end());
	}
	for(auto it : *obj) {
		// TODO: if there is no default value being set
		assert(it.second);
		it.first->Set(self, dynamic_cast<parserNode::Value*>(it.second)->GetValue(ctx), true);
	}
}

Class::Class(std::list<ilang::parserNode::Node*> *p, Iterable &iter, Context &ctx) {
	m_parents.reserve(p->size());
	Context self;
	self.scope = this;
	for(auto it : *p) {
		auto v = dynamic_cast<parserNode::Value*>(it)->GetValue(ctx);
		auto ptr = v->cast<Class*>();
		error(ptr, "Class can not inherit from non class");
		m_parents.push_back(v);
		// TODO: wrong
		// these are inheriting the variables from their parents, which means
		// the parents values can be set
		m_members.insert(ptr->begin(), ptr->end());
	}
	for(auto it : iter) {
		m_members.insert(make_pair(it.first, it.second));
	}
}

// TODO: this needs to be changed to getVariable
ValuePass Class::get(Context &ctx, Identifier i) {
	if(has(ctx, i)) {
		return Object_ish::get(ctx, i);
	}
	auto r = builtInGet(ctx, i);
	assert(r);
	return r;
}

ValuePass Class::builtInGet(Context &ctx, Identifier i) {
	if(i == Identifier("new")) {
		// new function
		// Calls create to create a new instance of the class,
		// and then calls init to set up the class with the passed arguments
		ValuePass create_fun = get(ctx, Identifier("create"));
		ilang::Function n([create_fun](Context &ctx, ilang::Arguments &args, ValuePass *ret) {
				ilang::Arguments na;
				*ret = create_fun->call(ctx, na);
				auto init = (*ret)->get(ctx, Identifier("init"));
				ValuePass ir = init->call(ctx, args);
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
						ValuePass v = hash->get(ctx, it.first);
						if(!v) {
							*ret = valueMaker(false);
							return;
						}
						it.second->Check(ctx, v);
					}
				} catch(BadType &e) {
					*ret = valueMaker(false);
					return;
				}
				*ret = valueMaker(true);
			});
		return valueMaker(inter);
	}
	return ValuePass();
	//assert(0); // so, not found
}




Class_instance::Class_instance(Handle<Class> c) : m_class(c) {
	assert(m_class);
	for(auto it : *m_class) {
		// should bind the item here??
		m_members.insert(make_pair(it.first, make_handle<Variable>(*it.second)));
	}
}

ValuePass Class_instance::get(Context &ctx, Identifier i) {
	if(Object_ish::has(ctx, i))
		return Object_ish::get(ctx, i);
	auto r = m_class->builtInGet(ctx, i);
	assert(r);
	error(r, "Class does not have member " << i.str());

	return r;
}


bool Class_instance::has(Context &ctx, Identifier i) {
	return Object_ish::has(ctx, i) ||
		i == "new" ||
		i == "create" ||
		i == "init" ||
		i == "instance" ||
		i == "interface";
}


Object::Object() {

}

Object::Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Context &ctx) : Object_ish() {
	Context ctx_to(ctx);
	ctx_to.scope = this;
	for(auto it : *obj) {
		error(!has(ctx, it.first->GetName()), "setting variable twice in object");
		assert(dynamic_cast<ilang::parserNode::Value*>(it.second));
		ValuePass val = dynamic_cast<ilang::parserNode::Value*>(it.second)->GetValue(ctx);
		it.first->Set(ctx_to, val);
	}
}

Object::Object(Iterable &iter, Context &ctx) {
	for(auto it : iter) {
		m_members.insert(make_pair(it.first, it.second));
	}
}

Array::Array(std::list<ilang::parserNode::Node*> *mods, std::list<ilang::parserNode::Node*> *elems, Context &ctx) {
	m_modifiers.reserve(mods->size());
	for(auto it : *mods) {
		m_modifiers.push_back(dynamic_cast<parserNode::Value*>(it)->GetValue(ctx));
	}
	m_members.reserve(elems->size());
	for(auto it : *elems) {
		auto var = make_handle<Variable>(ctx, m_modifiers);
		var->Set(ctx, dynamic_cast<parserNode::Value*>(it)->GetValue(ctx));
		m_members.push_back(var);
	}
}

Array::Array(std::vector<ValuePass> elems) {
	m_members.reserve(elems.size());
	for(auto it : elems) {
		m_members.push_back(make_variable(it));
	}
}

ValuePass Array::get(Context &ctx, Identifier i) {
	if(i.isInt()) { // this is an item in the array
		if(m_members.size() < i.raw()) {
			error(0, "array out of bound, length: " << m_members.size() << " and requesting element: " << i.raw());
			//assert(0); // TODO: raise an exception or something...
		}
		auto v = m_members.at(i.raw());
		auto val = v->Get(ctx);
		return val;
	}
	if(i == Identifier("length")) {
		return valueMaker(m_members.size());
	}
	if(i == Identifier("push")) {
		Handle<Array> self(this);
		Function p([self](Context &ctx, Arguments &args, ValuePass *ret) {
				auto v = make_handle<Variable>(ctx, self->m_modifiers);
				v->Set(ctx, args[0]);
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
				*ret = v->Get(ctx);
			});
		return valueMaker(p);
	}

	error(0, "Array does not have member " << i.str());
	// TODO: other array methods
}

void Array::set(Context &ctx, Identifier i, ValuePass v) {
	if(!i.isInt()) {
		error(0, "can not set non numeric type of " << i.str() << " to array type");
		assert(0); // trying to set some string type???
	}
	// TODO: if member already exists use that variable
	auto h = make_handle<Variable>(ctx, m_modifiers);
	h->Set(ctx, v);
	m_members[i.raw()] = h;
}

bool Array::has(Context &ctx, Identifier i) {
	if(i.isInt()) {
		return i.raw() < m_members.size();
	}
	return i == Identifier("length") ||
		i == Identifier("push") ||
		i == Identifier("pop");
}

Handle<Variable> Array::getVariable(Context &ctx, Identifier i) {
	if(!i.isInt()) {
		assert(0); // well there is basically no variable
	}
	if(i.raw() >= m_members.size()) {
		assert(0); // TODO: exception about not having...
	}
	auto h = m_members[i.raw()];
	return h;
}
