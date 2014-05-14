#include "function.h"
#include "scope.h"

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

void Arguments::populate(ScopePass scope, Function *func) {
	// TODO:
	if(func->func && func->func->params) {
		// there is some function with arguments that can be used to determine what to set values to
		auto it = func->func->params->begin(); // wtf was I thinking
		for(ValuePass v : pargs) {
			if(it == func->func->params->end()) break;

			// this needs better abstraction, as we shouldn't have this parserNode stuff here
			dynamic_cast<parserNode::Variable*>(*it)->Set(scope, ValuePass(v), true);
		}
}

ValuePass Arguments::get(int i) {
	return i < pargs.size() ? pargs.at(i) : ValuePass();
}

ValuePass Arguments::get(std::string s) {
	auto f = kwargs.find(s);
	return s != kwargs.end() ? f.second : ValuePass();
}



ValuePass Function::call(ScopePass scope, ilang::Arguments & args) {
	bool returned = false;
	ValuePass _ret = ValuePass();
	auto returnHandle = [&returned, &_ret] (ValuePass *ret) {
		returned = true;
		_ret = *ret;
	};
	ValuePass ret = ValuePass(new ilang::Value);
	//FunctionScope<decltype(returnHandle)> scope(_scope_made, _scope_self, returnHandle);
	// TODO: make this be on the stack instead of using new here
	auto fscope = new FunctionScope<decltype(returnHandle)>(contained_scope, scope, returnHandle);
	ScopePass scopep = ScopePass(fscope);
	args.populate(scope, this);

	if(native) {
		ptr(scopep, args.pargs, &ret);
	}else{
		ScopePass obj_scope = ScopePass();
		if(object_scope)
			obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(object_scope->Get())));
		ptr(scopep, args.pargs, &ret);
	}
	return ret;
}

Function Function::bind(ilang::ValuePass object) {
	// TODO: make this bind to a value
	return *this;
}

Function::Function(const Function &func) {
	native = func.native;
	ptr = func.ptr;
	contained_scope = func.contained_scope;
	object_scope = func.object_scope;
	this->func = func.func;
}

Function::Function(parserNode::Function *f, ScopePass scope, Function_ptr _ptr) {
	func = f;
	ptr = _ptr;
	contained_scope = scope;
	native = false;
}

Function::Function(Function_ptr _ptr) {
	ptr = _ptr;
	native = true;
}
