#ifndef _ilang_new_variable
#define _ilang_new_variable

#include "value.h"
#include <vector>

namespace ilang {
	class Variable_new {
	private:
		std::vector<ilang::ValuePass> m_modifiers;
		ilang::ValuePass m_value;
	public:
		Variable_new(std::vector<ilang::ValuePass> mod) : m_modifiers(mod) {}
		void set(ilang::ValuePass);
		ilang::ValuePass &get() { return m_value; }
	};
}

#endif
