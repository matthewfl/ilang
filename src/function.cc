#include "function.h"
#include "scope.h"
#include "error.h"

using namespace ilang;
using namespace std;

Arguments::Arguments() {
}

Arguments::Arguments(std::vector<ValuePass> pargs) {
	this->pargs = pargs;
}

void Arguments::push(ilang::ValuePass value) {
	pargs.push_back(value);
}

void Arguments::set(std::string key, ilang::ValuePass value) {
	kwargs.insert(std::pair<std::string, ValuePass>(key, value));
}

void Arguments::populate(Context &ctx, Function *func) {
	// TODO:
	if(func->func && func->func->params) {
		// there is some function with arguments that can be used to determine what to set values to
		auto it = func->func->params->begin(); // wtf was I thinking
		for(ValuePass v : pargs) {
			if(it == func->func->params->end()) break;
			// this needs better abstraction, as we shouldn't have this parserNode stuff here
			dynamic_cast<parserNode::Variable*>(*it)->Set(ctx, v, true);
			it++;
		}
	}
}

ValuePass Arguments::get(int i) {
	return i < pargs.size() ? pargs.at(i) : ValuePass();
}

ValuePass Arguments::get(std::string s) {
	auto f = kwargs.find(s);
	return f != kwargs.end() ? f->second : ValuePass();
}

size_t Arguments::size() {
	return pargs.size() + kwargs.size();
}

class ScopeFake : public Hashable {
private:
	std::map<Identifier, Handle<Variable> > *m_members;
	Hashable *m_parent;
	Context *m_ctx;
public:
	ScopeFake (std::map<Identifier, Handle<Variable> > *m, Context &ctx) : m_members(m), m_ctx(&ctx), m_parent(ctx.scope) {
		ctx.scope = this;
	}
	~ScopeFake() {
		assert(m_ctx->scope == this);
		m_ctx->scope = m_parent;
	}
	void set(Identifier i, ValuePass v) {
		auto it = m_members->find(i);
		if(it == m_members->end()) {
			error(m_parent, "unable to find variable " << i.str());
			return m_parent->set(i, v);
		}
		it->second->Set(v);
	}
	ValuePass get(Identifier i) {
		auto it = m_members->find(i);
		if(it == m_members->end()) {
			error(m_parent, "unable to find variable " << i.str())
			return m_parent->get(i);
		}
		return it->second->Get();
	}
	bool has(Identifier i) {
		return m_members->find(i) != m_members->end() || m_parent && m_parent->has(i);
	}
	Handle<Variable> getVariable(Identifier i) {
		auto it = m_members->find(i);
		if(it == m_members->end()) {
			error(m_parent, "unable to find variable " << i.str());
			return m_parent->getVariable(i);
		}
		return it->second;
	}
};



ValuePass Function::call(Context &ctx, ilang::Arguments & args) {
	//bool returned = false;
	//ValuePass _ret;
	// auto returnHandle = [&returned, &_ret] (ValuePass *ret) {
	// 	returned = true;
	// 	_ret = *ret;
	// };

	// ScopePass obj_scope = ScopePass();
	// if(object_scope)
	// 	obj_scope = ScopePass(new ObjectScope(object_scope->cast<Object*>().get()));

	//ValuePass ret;
	//FunctionScope<decltype(returnHandle)> scope(_scope_made, _scope_self, returnHandle);
	// TODO: make this be on the stack instead of using new here
	//auto fscope = new FunctionScope<decltype(returnHandle)>(contained_scope, obj_scope, returnHandle);
	//ScopePass scopep(fscope);
	//args.populate(scopep, this);
	ValuePass ret;
	ScopeFake bound_items(&m_bound, ctx);
	{

		Scope scope(ctx);
		args.populate(ctx, this);



		if(native) {
			ptr(ctx, args, &ret);
		}else{
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
	return ret;
}

IdentifierSet Function::UndefinedElements() {
	if(native)
		return IdentifierSet();
	return func->UndefinedElements();
}


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
	auto unbound = UndefinedElements();
	for(auto it : unbound) {
		cout << "looking to bind: " << it.str() << endl ;
		if(m_bound.find(it) == m_bound.end()) {
			auto vptr = h->getVariable(it);
			if(vptr) {
				m_bound.insert(pair<Identifier, Handle<Variable> >(it, vptr));
				cout << "was able to bind\n";
			} else {
				cout << "did not get var\n";
			}
		}
	}
}

Function::Function(const Function &func) {
	native = func.native;
	ptr = func.ptr;
	//contained_scope = func.contained_scope;
	//object_scope = func.object_scope;
	this->func = func.func;
	m_bound.insert(func.m_bound.begin(), func.m_bound.end());
	//for(auto it : func.m_bound)
	//	m_bound.insert(it->first, it->second);
}

Function::Function(parserNode::Function *f, Context &_ctx) { //, Function_ptr _ptr) {
	func = f;
	ptr = NULL;
	//ptr = _ptr;
	//contained_scope = scope;
	native = false;
	bind_self(_ctx.scope);
}

Function::Function(Function_ptr _ptr) {
	ptr = _ptr;
	native = true;
}

Function::Function () {
	// idk
	native = false;
	func = NULL;
}
