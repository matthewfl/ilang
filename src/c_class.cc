#include "ilang.h"

namespace ilang {
	// ValuePass Function_Creater( ValuePass (*fun)(Arguments&) ) {
	// 	auto fptr = [fun](Context &ctx, Arguments& args, ValuePass *ret) {
	// 		*ret = (*fun)(args);
	// 		assert(*ret);
	// 	};
	// 	ilang::Function f(fptr);
	// 	return valueMaker(f);
	// }
	ValuePass Function_Creater( ValuePass (*fun)(Context&, Arguments&) ) {
		auto fptr = [fun](Context &ctx, Arguments& args, ValuePass *ret) {
			*ret = (*fun)(ctx, args);
			assert(*ret);
		};
		ilang::Function f(fptr);
		return valueMaker(f);
	}

	// C_Class::get(Identifier i) {
	// 	if(has(i)) {
	// 		return Object_ish::get(i);
	// 	}

	// 	if(i == "new") {
	// 	}
	// 	if(i == "create") {
	// 	}
	// 	if(i == "init") {

	// 	}
	// 	if(i == "instance") {
	// 		Handle<C_Class> cls(this);
	// 		ilang::Function inst([cls](Context &ctx, ilang::Arguments &args, ValuePass *ret) {
	// 				*ret = valueMaker(cls->instance(args[0]));
	// 			});
	// 		return valueMaker(inst);
	// 	}
	// 	if(i == "interface") {
	// 		Handle<C_Class> cls(this);
	// 		ilang::Function inter([cls](Context &ctx, Arguments &args, ValuePass *ret) {
	// 				*ret = valueMaker(cls->interface(args[0]));
	// 			});
	// 		return valueMaker(inter);
	// 	}

	// }

	// ValuePass Class_Creater_class::get(Identifier i) {

	// ValuePass Function_Creater( ValuePass (*fun)(ScopePass, Arguments&) ) {
	// 	auto fptr = [fun](ScopePass scope, Arguments& args, ValuePass *ret) {
	// 		*ret = (*fun)(scope, args);
	// 		assert(*ret);
	// 	};
	// 	ilang::Function f(fptr);
	// 	return valueMaker(f);
	// }
	// C_Class::~C_Class() {
	// 	//std::cout << "---------------------deleting C_Class\n";
	// 	for(auto it : m_members) {
	// 		delete it.second;
	// 	}
	// }

	C_Class::C_Class() {}
	C_Class::~C_Class() {}

	bool C_Class::interface(ValuePass v) { return false; }
	bool C_Class::instance(ValuePass v) { return false; }
}
