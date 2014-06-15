#include "value.h"

using namespace ilang;

ValuePass_new::ValuePass_new(const ValuePass_new &x) {
	//x->type2();
	x->copyTo(m_data);
	//Get()->type2();
}

ValuePass_new::~ValuePass_new() {
	Get()->~Value_new();
}
