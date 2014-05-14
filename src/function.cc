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
}



ValuePass Function::call(ilang::Arguments & args) {
	bool returned = false;
	ValuePass _ret;
	auto returnHandle = [&returned, &_ret] (ValuePass *ret) {
		returned = true;
		_ret = *ret;
	};
	//FunctionScope<decltype(returnHandle)> scope(_scope_made, _scope_self, returnHandle);
	// TODO: make this be on the stack instead of using new here
	/*auto fscope = new FunctionScope<decltype(returnHandle)>(_scope_made, _scope_self, returnHandle);
	ScopePass scope = ScopePass(fscope);
	args.populate(scope, this);
	if(!native) {
		// execuite
	} else {

	}*/
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
