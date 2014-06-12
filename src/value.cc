#include "value.h"

using namespace ilang;

Value_new *ValuePass_new::Get() {
	(Value_new*)m_data;
}
