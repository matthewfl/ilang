#ifndef _ilang_new_variable
#define _ilang_new_variable

#include "debug.h"
#include "value.h"
#include <vector>

#include "helpers.h"
#include "handle.h"

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
		void Set(ilang::ValuePass);
		ilang::ValuePass &Get() { assert(m_value); return m_value; }
	};

	using Variable_ptr = Handle<Variable>;
}



#endif
