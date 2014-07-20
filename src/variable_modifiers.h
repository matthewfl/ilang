#ifndef _ilang_variable_modifiers
#define _ilang_variable_modifiers

#include "ilang.h"
#include "scope_new.h"
#include "variable_new.h"

namespace ilang {
	class Variable_modifier : public C_Class {
	public:

	};
}

#define ILANG_VARIABLE_MODIFIER(name, obj)															\
	namespace { struct _ILANG_VAR_MOD_##name {														\
			_ILANG_VAR_MOD_##name () {																				\
				::ilang::global_scope_register(Identifier( #name ),							\
			}} _ILANG_VAR_MOD_##name##_run;																		\
	}

#endif // _ilang_variable_modifiers
