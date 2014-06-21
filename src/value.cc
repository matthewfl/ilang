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

ValuePass_new ValuePass_new::operator + (ValuePass_new v) {
	return *Get() + v;
}
ValuePass_new ValuePass_new::operator - (ValuePass_new v) {
	return *Get() - v;
}
ValuePass_new ValuePass_new::operator * (ValuePass_new v) {
	return *Get() * v;
}
ValuePass_new ValuePass_new::operator / (ValuePass_new v) {
	return *Get() / v;
}


#define VALUE_MATH_OPS(cls, self, external_type)											 \
	ValuePass_new cls::preform_math_op(math_ops op, external_type v) {	 \
		switch (op) {																											 \
		case OP_add: return valueMaker( v + self );												 \
		case OP_substract: return valueMaker( v - self );									 \
		case OP_multiply: return valueMaker( v * self );									 \
		case OP_devide: return valueMaker( v / self );										 \
		default: assert(0);																								 \
		}																																	 \
	}

#define VALUE_MATH_ALL_OPS(cls, self)						\
	VALUE_MATH_OPS(cls, self, long)								\
	VALUE_MATH_OPS(cls, self, double)							\
	ValuePass_new cls::preform_math_op(math_ops op, std::string v) {			\
	  switch(op) {																												\
		case OP_add: {																											\
		  stringstream ss;																									\
			ss << v;																													\
			ss << self;																												\
			return valueMaker(ss.str());																			\
		}																																		\
		default: assert(0);																									\
	  }																																	 \
	}

VALUE_MATH_ALL_OPS(IntType, m_int)
VALUE_MATH_ALL_OPS(FloatType, m_float)

ValuePass_new StringType::preform_math_op(math_ops op, std::string v) {
	assert(op == OP_add);
	return valueMaker(v + GetSelf());
}
ValuePass_new StringType::preform_math_op(math_ops op, double v) {
	assert(op == OP_add);
	stringstream ss;
	ss << v;
	ss << GetSelf();
	return valueMaker(ss.str());
}
ValuePass_new StringType::preform_math_op(math_ops op, long v) {
	assert(op == OP_add);
	stringstream ss;
	ss << v;
	ss << GetSelf();
	return valueMaker(ss.str());
}




StringType::~StringType() {
	delete (std::string*)m_ptr;
}


// ValuePass_new StringType::preform_math_op(math_ops op, external_type v) {
// 	if(op != OP_add) RAISE_ERROR;
// 	return valueMaker(v + m_string);
// }
