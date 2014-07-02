#ifndef _ilang_new_variable
#define _ilang_new_variable

#include "value.h"
#include <vector>

#include "helpers.h"

namespace ilang {
	class Variable {
	private:

		//std::vector<ilang::Function> m_modifiers_set;
		//std::vector<ilang::Function> m_modifiers_get;
		std::vector<ilang::ValuePass> m_modifiers;
		ilang::ValuePass m_value;
	public:
		Variable(std::vector<ilang::ValuePass> mod) : m_modifiers(mod) {}
		Variable() {}
		void Set(ilang::ValuePass);
		ilang::ValuePass &Get() { return m_value; }
	};
}

#endif
