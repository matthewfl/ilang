#ifndef _ilang_new_variable
#define _ilang_new_variable

#include "debug.h"
#include "value.h"
#include <vector>

#include "helpers.h"
#include "handle.h"
#include "error.h"

namespace ilang {
	class Variable : public Handle_base {
	private:

		//std::vector<ilang::Function> m_modifiers_set;
		//std::vector<ilang::Function> m_modifiers_get;
		std::vector<ilang::ValuePass> m_modifiers;
		ilang::ValuePass m_value;
	public:
		Variable(std::vector<ilang::ValuePass> mod);
		Variable() {}
		Variable(const Variable& v);
		void Set(ilang::ValuePass);
		ilang::ValuePass & Get() {
			error(m_value, "Can't use a variable before it is set");
			return m_value;
		}
		void Check(ilang::ValuePass);
		void SetModifiers(std::vector<ilang::ValuePass> mod);
	};

	using Variable_ptr = Handle<Variable>;

	inline Variable_ptr make_variable(const ValuePass &val) {
		auto var = make_handle<Variable>();
		var->Set(val);
		return var;
	}

}



#endif
