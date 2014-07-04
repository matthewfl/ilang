#include "value.h"
#include "value_types.h"
#include "function.h"
#include "hashable.h"
#include "object.h"

using namespace ilang;
using namespace std;

ValuePass::ValuePass(const ValuePass &x) {
	//x->type2();
	if(*(long*)x.m_data != 0)
		x->copyTo(m_data);
	else
		*(long*)m_data = 0;
	//Get()->type2();
}

ValuePass::~ValuePass() {
	if(*(long*)m_data != 0)
		Get()->~Value_new();
}

ValuePass ValuePass::operator + (ValuePass v) {
	return *Get() + v;
}
ValuePass ValuePass::operator - (ValuePass v) {
	return *Get() - v;
}
ValuePass ValuePass::operator * (ValuePass v) {
	return *Get() * v;
}
ValuePass ValuePass::operator / (ValuePass v) {
	return *Get() / v;
}


#define VALUE_MATH_OPS(cls, self, external_type)											 \
	ValuePass cls::preform_math_op(math_ops op, external_type v) {	 \
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
	ValuePass cls::preform_math_op(math_ops op, std::string v) {			\
	  switch(op) {																												\
		case OP_add: {																											\
		  std::stringstream ss;																							\
			ss << v;																													\
			ss << self;																												\
			return valueMaker(ss.str());																			\
		}																																		\
		default: assert(0);																									\
	  }																																	 \
	}

VALUE_MATH_ALL_OPS(IntType, m_int)
VALUE_MATH_ALL_OPS(FloatType, m_float)

ValuePass StringType::preform_math_op(math_ops op, std::string v) {
	assert(op == OP_add);
	return valueMaker(v + GetSelf());
}
ValuePass StringType::preform_math_op(math_ops op, double v) {
	assert(op == OP_add);
	stringstream ss;
	ss << v;
	ss << GetSelf();
	return valueMaker(ss.str());
}
ValuePass StringType::preform_math_op(math_ops op, long v) {
	assert(op == OP_add);
	stringstream ss;
	ss << v;
	ss << GetSelf();
	return valueMaker(ss.str());
}
StringType::~StringType() {
	delete (std::string*)m_ptr;
}

const std::type_info &FunctionType::type() { return typeid(ilang::Function); }
FunctionType::FunctionType(const ilang::Function &f) {
	m_ptr = new ilang::Function(f);
}
FunctionType::FunctionType(const FunctionType &f) {
	m_ptr = new ilang::Function(*(ilang::Function*)f.m_ptr);
}
// FunctionType::FunctionType(FunctionType &&f) {
// 	m_ptr = f.m_ptr;
// 	f.m_ptr = NULL;
// }
FunctionType::~FunctionType() {
	delete (ilang::Function*)m_ptr;
}
void FunctionType::copyTo(void *d) {
	new (d) FunctionType(*this);
}
ValuePass FunctionType::call(ilang::Arguments &args) {
	Context ctx;
	ilang::ValuePass gg = ((ilang::Function*)m_ptr)->call(ctx, args);
	return gg;
}


HashableType::HashableType(std::shared_ptr<Hashable> h) {
	m_ptr = new std::shared_ptr<Hashable>(h);
}
HashableType::HashableType(std::shared_ptr<Object> o) {
	auto h = std::dynamic_pointer_cast<Hashable>(o);
	m_ptr = new std::shared_ptr<Hashable>(h);
}
HashableType::HashableType(std::shared_ptr<Array> a) {
	auto h = std::dynamic_pointer_cast<Hashable>(a);
	m_ptr = new std::shared_ptr<Hashable>(h);
}
HashableType::HashableType(const HashableType &h) {
	m_ptr = new std::shared_ptr<Hashable>(*(shared_ptr<Hashable>*)h.m_ptr);
}
HashableType::~HashableType() {
	delete (std::shared_ptr<Hashable>*)m_ptr;
}
const std::type_info& HashableType::type() { return typeid(Hashable*); }
std::shared_ptr<Hashable> HashableType::Cast(cast_chooser<Hashable*> c) {
	return *(std::shared_ptr<Hashable>*)m_ptr;
}
void HashableType::copyTo(void *d) {
	new (d) HashableType(*this);
}

ClassType::ClassType(std::shared_ptr<Class> c) {
	m_ptr = new std::shared_ptr<Class>(c);
}
ClassType::ClassType(const ClassType &c) {
	m_ptr = new std::shared_ptr<Class>(*(shared_ptr<Class>*)c.m_ptr);
}
ClassType::~ClassType() {
	delete (std::shared_ptr<Class>*)m_ptr;
}
const std::type_info& ClassType::type() { return typeid(Class*); }
void ClassType::copyTo(void *d) {
	new (d) ClassType(*this);
}
std::shared_ptr<Class> ClassType::Cast(cast_chooser<Class*> c) {
	return *(std::shared_ptr<Class>*)m_ptr;
}
