#ifndef _ilang_value
#define _ilang_value

#include <typeinfo>
#include <type_traits>
#include <string>
#include <string.h>

#include "debug.h"

#include <iostream>
using namespace std;

namespace ilang {

	class Value_new;

	class ValuePass_new {
  private:
    char m_data[32]; // TODO: don't hard code the size // sizeof(Value_new), should be 16
	public:
		inline Value_new *Get() const { return (Value_new*)m_data; }
    inline Value_new *operator->() const { return Get(); }

    ValuePass_new() {
			// TODO: should have something here to make sure there are no problems
			//new (m_data) Value_new;
    }
		template <typename T>ValuePass_new (T t) {
			assert(sizeof(T) <= sizeof(m_data));
			new (m_data) T(t);
		}
		ValuePass_new(const ValuePass_new &x);
		~ValuePass_new();

		// proxy functions
		ValuePass_new operator + (ValuePass_new v); // { return *Get() + v; }
		ValuePass_new operator - (ValuePass_new v); // { return *Get() - v; }
		ValuePass_new operator * (ValuePass_new v); // { return *Get() * v; }
		ValuePass_new operator / (ValuePass_new v); // { return *Get() / v; }
  };


	// TODO: change to raise an exception that is caught
#define RAISE_ERROR {assert(0);}

	enum math_ops {
		OP_add,
		OP_substract,
		OP_multiply,
		OP_devide
	};

	class Arguments;
	class callable_virtals_mixin {
	public:
		virtual ValuePass_new call(ilang::Arguments &a) RAISE_ERROR;
	};

	class Identifier;
	class hashable_virtuals_mixin {
	public:
		virtual ValuePass_new get(ilang::Identifier &i) RAISE_ERROR;
		virtual void set(ilang::Identifier &i, ValuePass_new &v) RAISE_ERROR;
	};

	template <typename T> class cast_chooser {
		friend class Value_new;
		explicit cast_chooser() {};
	};


  class Value_new// :
		//public math_virtuals_mixin//,
	 //public cast_mixin
	{
		friend class ValuePass_new;
	public:  // yolo
    union {
      long m_int;
      double m_float;
			bool m_bool;
			char *m_str;
			long m_identifier;
		};

  public:
		virtual ~Value_new() {}
		virtual const std::type_info &type()=0;

	protected:
		virtual void copyTo(void *d) { memcpy(d, this, sizeof(Value_new)); }

		// cast mixins
		virtual int Cast(cast_chooser<int> c) RAISE_ERROR;
		virtual long Cast(cast_chooser<long> c) RAISE_ERROR;
		virtual float Cast(cast_chooser<float> c) RAISE_ERROR;
		virtual double Cast(cast_chooser<double> c) RAISE_ERROR;
		virtual bool Cast(cast_chooser<bool> C) RAISE_ERROR;

	public:
		template <typename T> T cast() {
			cast_chooser<T> c;
			return Cast(c);
		}

		// math mixins
	public:
		virtual ValuePass_new operator + (ValuePass_new v) RAISE_ERROR;
		virtual ValuePass_new operator - (ValuePass_new v) RAISE_ERROR;
		virtual ValuePass_new operator * (ValuePass_new v) RAISE_ERROR;
		virtual ValuePass_new operator / (ValuePass_new v) RAISE_ERROR;
		virtual ValuePass_new preform_math_op(math_ops op, long v) RAISE_ERROR;
		virtual ValuePass_new preform_math_op(math_ops op, double v) RAISE_ERROR;
		//virtual ValuePass_new preform_math_op(math_ops op, bool v) RAISE_ERROR;

	};


#define VALUE_MATH_CLS_MIXIN(self)																			\
	virtual ValuePass_new operator + (ValuePass_new v) { return v->preform_math_op(OP_add, self); } \
	virtual ValuePass_new operator - (ValuePass_new v) { return v->preform_math_op(OP_substract, self); } \
	virtual ValuePass_new operator * (ValuePass_new v) { return v->preform_math_op(OP_multiply, self); } \
	virtual ValuePass_new operator / (ValuePass_new v) { return v->preform_math_op(OP_devide, self); } \
	virtual ValuePass_new preform_math_op(math_ops op, long v);						\
	virtual ValuePass_new preform_math_op(math_ops op, double v);
	//	virtual ValuePass_new preform_math_op(math_ops op, std::string v);


	class IntType : public Value_new {
	public:
		IntType(int i) { m_int = i; }
		IntType(long i) { m_int = i; }
		ValuePass_new operator + (ValuePass_new &v) {
			return IntType(m_int + v->m_int);
		}
		~IntType() {}
		virtual const std::type_info &type() { return typeid(long); }

	protected:
		virtual int Cast(cast_chooser<int> c) { return m_int; }
		virtual long Cast(cast_chooser<long> c) { return m_int; }
		virtual float Cast(cast_chooser<float> c) { return m_int; }
		virtual double Cast(cast_chooser<double> c) { return m_int; }
		virtual bool Cast(cast_chooser<bool> c) { return m_int != 0; }


	public:
		VALUE_MATH_CLS_MIXIN(m_int);
	};

	class FloatType : public Value_new {
	public:
		FloatType(double f) { m_float = f; }
		ValuePass_new operator + (ValuePass_new &v) {
			return FloatType(m_float + 2);//v->m_float);
		}
		virtual const std::type_info &type() { return typeid(double); }

	protected:
		virtual int Cast(cast_chooser<int> c) { return m_float; }
		virtual long Cast(cast_chooser<long> c) { return m_float; }
		virtual float Cast(cast_chooser<float> c) { return m_float; }
		virtual double Cast(cast_chooser<double> c) { return m_float; }
		virtual bool Cast(cast_chooser<bool> c) { return m_float != 0; }

	public:
		VALUE_MATH_CLS_MIXIN(m_float);
	};

	class BoolType : public Value_new {
	public:
		BoolType(bool b) { m_bool = b; }
		const std::type_info &type() { return typeid(bool); }
	protected:
		virtual int Cast(cast_chooser<int> c) { return m_bool; }
		virtual long Cast(cast_chooser<long> c) { return m_bool; }
		virtual float Cast(cast_chooser<float> c) { return m_bool; }
		virtual double Cast(cast_chooser<double> c) { return m_bool; }
		virtual bool Cast(cast_chooser<bool> c) { return m_bool; }
	};

	// class StringType : public Value_new {
	// public:
	// 	StringType(char *str) {}
	// 	StringType(std::string str) {}

	// 	virtual const std::type_info &type() { return typeid(std::string); }
	// };

	// I suppose there is no need for this as there could just be a single function
	// with overloaded parameters

	template<typename Of, typename To, typename... Others> struct _valueMaker { //: _valueMaker<Others...> {
		template <typename T> static inline ValuePass_new create(T t) {
			return _valueMaker<Others...>::create(t);
		}
		static inline ValuePass_new create(Of t) {
			//cout << "creating new " << typeid(To).name() << endl;
			return ValuePass_new(To(t));
		}
		template <typename T> inline ValuePass_new operator () (T t) {
			return create(t);
		}
	};

	static auto valueMaker = _valueMaker<
		int, IntType,
		long, IntType,
		double, FloatType,
		float, FloatType,
		bool, BoolType//,
		// char*, StringType,
		// std::string, StringType
		>();

}

#endif // _ilang_value
