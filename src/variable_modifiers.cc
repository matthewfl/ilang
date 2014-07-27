#include "variable_modifiers.h"

using namespace ilang;

ILANG_VARIABLE_CHECKER(Int) {
	return var->type() == typeid(long);
}

ILANG_VARIABLE_CHECKER(Float) {
	return var->type() == typeid(double);
}

ILANG_VARIABLE_CHECKER(String) {
	return var->type() == typeid(std::string);
}

ILANG_VARIABLE_CHECKER(Bool) {
	return var->type() == typeid(bool);
}

ILANG_VARIABLE_CHECKER(Number) {
	return var->type() == typeid(long) || var->type() == typeid(double);
}

ILANG_VARIABLE_CHECKER(Function) {
	// TODO: make this more advance to it can check the type params of the function
	return var->type() == typeid(Function*);
}

ILANG_VARIABLE_CHECKER(Object) {
	return var->type() == typeid(Hashable*);
}

ILANG_VARIABLE_CHECKER(Class) {
	if(var->type() != typeid(Hashable*))
		return false;
	auto h = var->cast<Hashable*>();
	return dynamic_cast<Class*>(h.get());
}

ILANG_VARIABLE_CHECKER(Array) {
	if(var->type() != typeid(Hashable*))
		return false;
	auto h = var->cast<Hashable*>();
	return dynamic_cast<Array*>(h.get());
}