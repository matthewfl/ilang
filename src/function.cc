#include "function.h"
#include "error.h"
#include "value_types.h"

using namespace ilang;
using namespace std;

Arguments::Arguments() {
}

Arguments::Arguments(const Arguments &o) {
	m_args = o.m_args;
	m_next_identifier = o.m_next_identifier;
}

Arguments::Arguments(std::vector<ValuePass> pargs) {
	for(auto it : pargs)
		push(it);
	//this->pargs = pargs;
}

void Arguments::push(ilang::ValuePass value) {
	m_args.insert(make_pair(Identifier(m_next_identifier++), value));
								//pargs.push_back(value);
}

void Arguments::set(Context &ctx, Identifier key, ilang::ValuePass value) {
	m_args.insert(make_pair(key, value));
	//kwargs.insert(std::pair<std::string, ValuePass>(key, value));
}

void Arguments::populate(Context &ctx, Function *func) {
	// TODO:
	if(!func->native && func->func->params) {
		// there is some function with arguments that can be used to determine what to set values to

		auto it = func->func->params->begin(); // wtf was I thinking
		auto end = func->func->params->end();
		for(auto ait : m_args) {
			if(it == end) break;
			if(ait.first.isInt()) {
				dynamic_cast<parserNode::Variable*>(*it)->Set(ctx, ait.second, true);
			}
			it++;
		}

		// for(ValuePass v : pargs) {
		// 	if(it == func->func->params->end()) break;
		// 	// this needs better abstraction, as we shouldn't have this parserNode stuff here
		// 	dynamic_cast<parserNode::Variable*>(*it)->Set(ctx, v, true);
		// 	it++;
		// }
	}
}

ValuePass Arguments::get(Context &ctx, Identifier i) {
	auto it = m_args.find(i);
	if(it != m_args.end()) {
		return it->second;
	}
	if(i == "length") {
		return valueMaker(size());
	}
	assert(0);
}

bool Arguments::has(Context &ctx, Identifier i) {
	return m_args.find(i) != m_args.end();
}

// ValuePass Arguments::get(int i) {
// 	// TODO: should this raise an exception
// 	return i < pargs.size() ? pargs.at(i) : ValuePass();
// }

// ValuePass Arguments::get(std::string s) {
// 	auto f = kwargs.find(s);
// 	return f != kwargs.end() ? f->second : ValuePass();
// }

size_t Arguments::size() {
	return m_args.size();
	//return pargs.size() + kwargs.size();
}

Handle<Variable> Arguments::getVariable(Context &ctx, Identifier i) {
	return make_variable(get(ctx, i));
}

class ScopeFake : public Hashable {
private:
	std::map<Identifier, Handle<Variable> > *m_members;
	Hashable *m_parent;
	Context *m_ctx;
public:
	ScopeFake (std::map<Identifier, Handle<Variable> > *m, Context &ctx) : m_members(m), m_ctx(&ctx), m_parent(ctx.scope) {
		ctx.scope = this;
		assert(m_members);
		for(auto v : *m_members) {
			assert(v.second);
			// if the variable isn't set by this point then it must be used
			// before it is getting referenced.
			auto val = v.second->Get(ctx);
			assert(val);
			if(val->type() == typeid(Function)) {
				val->cast<Function*>()->vvv();
			}
		}
	}
	~ScopeFake() {
		assert(m_ctx->scope == this);
		m_ctx->scope = m_parent;
	}
	void set(Context &ctx, Identifier i, ValuePass v) {
		auto it = m_members->find(i);
		if(it == m_members->end()) {
			error(m_parent, "unable to find variable " << i.str());
			return m_parent->set(ctx, i, v);
		}
		it->second->Set(ctx, v);
	}
	ValuePass get(Context &ctx, Identifier i) {
		auto it = m_members->find(i);
		if(it == m_members->end()) {
			error(m_parent, "unable to find variable " << i.str());
			return m_parent->get(ctx, i);
		}
		//		Function fun;
		//return valueMaker(fun);
		ValuePass ret = it->second->Get(ctx);
		if(ret->type() == typeid(Function)) {
			ret->cast<ilang::Function*>()->vvv();
		}
		return ret;
	}
	bool has(Context &ctx, Identifier i) {
		return m_members->find(i) != m_members->end() || m_parent && m_parent->has(ctx, i);
	}
	Handle<Variable> getVariable(Context &ctx, Identifier i) {
		auto it = m_members->find(i);
		if(it == m_members->end()) {
			if(!m_parent) {
				//error(m_parent, "unable to find variable " << i.str());
				return NULL;
			}
			return m_parent->getVariable(ctx, i);
		}
		return it->second;
	}
};

void Function::vvv() {
	// TODO: remove
	assert((native && ptr) || ((long)func > 0x1000));
}


ValuePass Function::call(Context &ctx, ilang::Arguments & args) {
	ValuePass ret;
	try {
		ScopeFake bound_items(&m_bound, ctx);
		{

			Scope scope(ctx);
			args.populate(ctx, this);

			// TODO: use std::move here?
			ctx.scope->set(ctx, "arguments", valueMaker(make_handle<Arguments>(args)));

			if(native) {
				ptr(ctx, args, &ret);
			}else{
				func->PreRegister(ctx);
				if(func->body) {
					for(auto n : *func->body) {
						if(ctx.returned) break;
						assert(n);
						n->Run(ctx);
					}
				}
				// this seems bad....
				// thinking that using excpetions to break out might
				// be a good?? idea
				if(ctx.returned && ctx.returned != (ValuePass*)1) {
					ret = *ctx.returned;
					delete ctx.returned;
				}
				ctx.returned = NULL;
				//ptr(scopep, args.pargs, &ret);
			}
		}
	} catch(BadType &e) {
		// should this only apply to when checking the arguments??
		if(m_alternate) {
			ret = m_alternate->call(ctx, args);
		} else {
			throw;
		}
	}
	return ret;
}

IdentifierSet Function::UndefinedElements() {
	if(native)
		return IdentifierSet();
	return func->UndefinedElements();
}

// TODO: there need to be 2 different types of bind
// 1. bind which only binds non bound variables
// 2. bind which will override the bind on variables

Function Function::bind(ilang::ValuePass object) {
	// TODO: make this bind to a value
	auto h = object->cast<Hashable*>();
	return bind(h.get());
}

Function Function::bind(Context &ctx) {
  return bind(ctx.scope);
}

Function Function::bind(Hashable* h) {
	Function ret(*this);
	ret.bind_self(h);
	return ret;
}

void Function::bind_self(Hashable *h) {
	Context ctx; // the context isn't going to be used since it is just getting the variable references
	auto unbound = UndefinedElements();
	for(auto it : unbound) {
		//cout << "looking to bind: " << it.str() << endl ;
		if(m_bound.find(it) == m_bound.end()) {
			auto vptr = h->getVariable(ctx, it);
			if(vptr) {
				m_bound.insert(pair<Identifier, Handle<Variable> >(it, vptr));
				//cout << "was able to bind\n";
			} else {
				//cout << "did not get var\n";
			}
		}
	}
	// this is somewhat inefficent....
	if(m_alternate) {
		ValuePass temp = valueMaker(m_alternate->cast<ilang::Function*>()->bind(h));
		m_alternate = temp;
	}
}

Function Function::alternate(ilang::ValuePass alt) {
	// TODO: this is wrong, as there might already be an alternate set
	assert(alt->type() == typeid(Function));
	Function ret(*this);
	if(ret.m_alternate) {
		auto f = ret.m_alternate->cast<Function*>()->alternate(alt);
		ret.m_alternate = valueMaker(f);
	} else {
		ret.m_alternate = alt;
	}
	return ret;
}

Function::Function(const Function &func) {
	assert(this != &func);
	const_cast<ilang::Function*>(&func)->vvv();
	assert(func.native || func.func);
	native = func.native;
	ptr = func.ptr;
	this->func = func.func;
	m_bound.insert(func.m_bound.begin(), func.m_bound.end());
	m_alternate = func.m_alternate;
	vvv();
}

Function::~Function() {
	func = NULL;
	ptr = NULL;
	native = false;
	//cout << "delete function " << this << endl << flush;
}

Function::Function(parserNode::Function *f, Context &_ctx) { //, Function_ptr _ptr) {
	func = f;
	ptr = NULL;
	//ptr = _ptr;
	//contained_scope = scope;
	native = false;
	bind_self(_ctx.scope);
	vvv();
}

Function::Function(Function_ptr _ptr) {
	ptr = _ptr;
	native = true;
}

Function::Function () {
	// idk
	native = true;
	ptr = [](Context &ctx, Arguments &args, ValuePass *ret) {
		error(0, "Called empty function");
	};
	func = (parserNode::Function*)123;
}
