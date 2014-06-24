#ifndef _ilang_new_variable
#define _ilang_new_variable

#include "value.h"
#include <vector>

namespace ilang {
	class Variable_new {
	private:
		std::vector<ilang::ValuePass_new> m_modifiers;
		ilang::ValuePass_new m_value;
	public:
		Variable_new(std::vector<ilang::ValuePass_new> mod) : m_modifiers(mod) {}
		void set(ilang::ValuePass_new);
		ilang::ValuePass_new &get() { return m_value; }
	};
}

#endif
