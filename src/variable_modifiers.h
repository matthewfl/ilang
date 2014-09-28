#ifndef _ilang_variable_modifiers
#define _ilang_variable_modifiers

#include "ilang.h"
#include "scope_new.h"
#include "variable_new.h"
#include "value_types.h"
#include "error.h"

// namespace ilang {
// 	class Variable_modifier : public C_Class {
// 	public:

// 	};
// }

// #define ILANG_VARIABLE_MODIFIER(name, obj)															\
// 	namespace { struct _ILANG_VAR_MOD_##name {														\
// 			_ILANG_VAR_MOD_##name () {																				\
// 				::ilang::global_scope_register(Identifier( #name ),							\
// 			}} _ILANG_VAR_MOD_##name##_run;																		\
// 	}

#define ILANG_VARIABLE_CHECKER(name)																		\
	static bool _ilang_variable_checker_fun_##name (ilang::ValuePass var) ; \
	namespace { struct _ILANG_VAR_CHK_##name {														\
	_ILANG_VAR_CHK_##name () {																						\
	ilang::Function fun([](ilang::Context &ctx, ilang::Arguments &args, ilang::ValuePass *ret) { \
	error(args.size() == 1, "Variable check " #name " only takes 1 argument"); \
	ilang::ValuePass v = args[0];																					\
	bool r = _ilang_variable_checker_fun_##name (v) ;											\
		*ret = ilang::valueMaker(r);																				\
		});																																	\
	::ilang::global_scope_register(ilang::Identifier( #name ),	ilang::valueMaker(fun)); \
	} } _ILANG_VAR_CHK_INST_##name ;																			\
	}																																			\
	bool _ilang_variable_checker_fun_##name (ilang::ValuePass var)


#endif // _ilang_variable_modifiers
