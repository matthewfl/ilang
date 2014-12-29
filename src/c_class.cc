#include "ilang.h"

namespace ilang {
	ValuePass Function_Creater( ValuePass (*fun)(Context&, Arguments&) ) {
		auto fptr = [fun](Context &ctx, Arguments& args, ValuePass *ret) {
			*ret = (*fun)(ctx, args);
			assert(*ret);
		};
		ilang::Function f(fptr);
		return valueMaker(f);
	}

	C_Class::C_Class() {}
	C_Class::~C_Class() {}

	bool C_Class::interface(ValuePass v) { return false; }
	bool C_Class::instance(ValuePass v) { return false; }
}
