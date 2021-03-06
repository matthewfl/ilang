#include "value.h"
#include "value_types.h"
#include "function.h"
#include "hashable.h"
#include "object.h"
#include "tuple.h"

using namespace ilang;
using namespace std;

ValuePass::ValuePass(const ValuePass &x) {
	if(*(long*)x.m_data != 0)
		x->copyTo(m_data);
	else
		*(long*)m_data = 0;
}

ValuePass::ValuePass(Value *v) {
	v->copyTo(m_data);
}

ValuePass::~ValuePass() {
	if(*(long*)m_data != 0)
		Get()->~Value();
	*(long*)m_data = 0;
}

void ValuePass::operator = (const ValuePass &v) {
  ValuePass s(v);
	s.swap(*this);
}

void ValuePass::swap(ValuePass &v) {
	std::swap(m_data, v.m_data);
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
bool ValuePass::operator == (ValuePass v) {
	return *Get() == v;
}
bool ValuePass::operator <= (ValuePass v) {
	return *Get() <= v;
}
bool ValuePass::operator < (ValuePass v) {
	return *Get() < v;
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

#define VALUE_LOGIC_OPS(cls, self, external_type)							\
	bool cls::preform_logic_op(logic_ops op, external_type v) { \
	switch(op) {																								\
	case OP_lessthan: return v < self;													\
	case OP_lessequal: return v <= self;												\
	case OP_equal: return v == self;														\
	default: assert(0);																					\
	}																														\
	}

#define VALUE_LOGIC_ALL_OPS(cls, self)				 \
	VALUE_LOGIC_OPS(cls, self, long)						 \
	VALUE_LOGIC_OPS(cls, self, double)					 \
	VALUE_LOGIC_OPS(cls, self, bool)						 \
	bool cls::preform_logic_op(logic_ops op, std::string v) {			\
		std::stringstream ss; ss << self; std::string s = ss.str();	\
		switch(op) {																								\
		case OP_lessthan: return v < s;															\
		case OP_lessequal: return v <= s;														\
		case OP_equal: return v == s;																\
		default: assert(0);																					\
		}																														\
	}


VALUE_LOGIC_ALL_OPS(IntType, m_int);
VALUE_LOGIC_ALL_OPS(FloatType, m_float);
VALUE_LOGIC_ALL_OPS(BoolType, m_bool);

#define VALUE_LOGIC_OPS_STRING(external_type) \
	bool StringType::preform_logic_op(logic_ops op, external_type v) { \
		std::stringstream ss; ss << v; std::string s = ss.str();			\
		switch(op) {																									\
		case OP_lessthan: return s < GetSelf();												\
		case OP_lessequal: return s <= GetSelf();											\
		case OP_equal: return s == GetSelf();													\
		default: assert(0);																						\
		}																															\
	}

VALUE_LOGIC_OPS_STRING(long)
VALUE_LOGIC_OPS_STRING(double)
VALUE_LOGIC_OPS_STRING(std::string)
VALUE_LOGIC_OPS_STRING(bool)



const std::type_info &FunctionType::type() { return typeid(ilang::Function); }  // TODO: this should really be Function*
FunctionType::FunctionType(const ilang::Function &f) {
	m_ptr = new ilang::Function(f);
}
FunctionType::FunctionType(const FunctionType &f) {
	auto fp = (ilang::Function*)f.m_ptr;
	m_ptr = new ilang::Function(*fp);
}
FunctionType::~FunctionType() {
	delete (ilang::Function*)m_ptr;
	m_ptr = NULL;
}
void FunctionType::copyTo(void *d) {
	new (d) FunctionType(*this);
}
ValuePass FunctionType::call(Context &ctx, ilang::Arguments &args) {
	ilang::ValuePass gg = ((ilang::Function*)m_ptr)->call(ctx, args);
	return gg;
}
ValuePass FunctionType::operator + (ValuePass v) {
	assert(v->type() == typeid(Function)); // should raise some bad operation exception...
	Function ret = ((ilang::Function*)m_ptr)->alternate(v);
	return valueMaker(ret);
}


HashableType::HashableType(Handle<Hashable> h) {
	m_ptr = new Handle<Hashable>(h);
}
HashableType::HashableType(const HashableType &h) {
	m_ptr = new Handle<Hashable>(*(Handle<Hashable>*)h.m_ptr);
}
HashableType::~HashableType() {
	delete (Handle<Hashable>*)m_ptr;
}
const std::type_info& HashableType::type() { return typeid(Hashable*); }
Handle<Hashable> HashableType::Cast(cast_chooser<Hashable*> c) {
	return *(Handle<Hashable>*)m_ptr;
}
Handle<Class> HashableType::Cast(cast_chooser<Class*> c) {
	auto p = *(Handle<Hashable>*)m_ptr;
	return dynamic_pointer_cast<Class>(p);
}
Handle<Tuple> HashableType::Cast(cast_chooser<Tuple*> c) {
	auto p = *(Handle<Hashable>*)m_ptr;
	return dynamic_pointer_cast<Tuple>(p);
}
void HashableType::copyTo(void *d) {
	new (d) HashableType(*this);
}
ValuePass HashableType::get(Context &ctx, Identifier key) {
	return (*(Handle<Hashable>*)m_ptr)->get(ctx, key);
}
void HashableType::set(Context &ctx, Identifier key, ValuePass value) {
	(*(Handle<Hashable>*)m_ptr)->set(ctx, key, value);
}

ClassType::ClassType(Handle<Class> c) {
	m_ptr = new Handle<Class>(c);
}
ClassType::ClassType(const ClassType &c) {
	m_ptr = new Handle<Class>(*(Handle<Class>*)c.m_ptr);
}
ClassType::~ClassType() {
	delete (Handle<Class>*)m_ptr;
}
const std::type_info& ClassType::type() { return typeid(Class*); }
void ClassType::copyTo(void *d) {
	new (d) ClassType(*this);
}
Handle<Class> ClassType::Cast(cast_chooser<Class*> c) {
	return *(Handle<Class>*)m_ptr;
}

VariableType::VariableType(types t) {
	m_int = t;
}

const std::type_info & VariableType::type() {
	return typeid(VariableType);
}

VariableType::types VariableType::getType() {
	return (types) m_int;
}
