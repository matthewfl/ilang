#include "value.h"

using namespace ilang;

Value_new *ValuePass_new::Get() {
	return (Value_new*)m_data;
	//return m_ptr;
}
