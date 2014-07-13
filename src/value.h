#ifndef _ilang_value
#define _ilang_value

#include <typeinfo>
#include <type_traits>
#include <string>
#include <sstream>
#include <memory>
#include <string.h>

#include "debug.h"

#include <iostream>
//using namespace std;

#include "handle.h"
#include "identifier.h"


namespace ilang {

	class Value_new;
	class Function;
	class Arguments;
	class Object;
	class Array;
	class Identifier;
	class Class;
	class Hashable;

	typedef shared_ptr<Object> Object_ptr;


	class ValuePass {
  private:
    char m_data[32]; // TODO: don't hard code the size // sizeof(Value_new), should be 16
	public:
		inline Value_new *Get() const {
			assert(*(long*)m_data);
			return (Value_new*)m_data;
		}
    inline Value_new *operator->() const { return Get(); }
		inline Value_new &operator*() const { return *Get(); }

    ValuePass() {
			// TODO: should have something here to make sure there are no problems
			*(long*)m_data = 0;
			//new (m_data) Value_new;
    }
		template <typename T>ValuePass (T t) {
			assert(sizeof(T) <= sizeof(m_data));
			assert(dynamic_cast<Value_new*>(&t));
			new (m_data) T(t);
		}
		// TODO: xvalue constructure
		ValuePass(const ValuePass &x);
		~ValuePass();

		// proxy functions
		ValuePass operator + (ValuePass v); // { return *Get() + v; }
		ValuePass operator - (ValuePass v); // { return *Get() - v; }
		ValuePass operator * (ValuePass v); // { return *Get() * v; }
		ValuePass operator / (ValuePass v); // { return *Get() / v; }

		bool operator == (ValuePass v);
		bool operator != (ValuePass v) { return !operator==(v); }
		bool operator <= (ValuePass v);
		bool operator >= (ValuePass v) { return !operator<(v); }
		bool operator < (ValuePass v);
		bool operator > (ValuePass v) { return !operator<=(v); }

		operator bool () const { return *(long*)m_data != 0; }

		ValuePass call(ilang::Arguments &a); // { return Get()->call(a); }
		//ValuePass operator () (ilang::Arguments &a) { return call(a); }
		//template<typename... types> ValuePass operator() (types... values);

	};

	//using ValuePass = ValuePass;


	// TODO: change to raise an exception that is caught
#define RAISE_ERROR {assert(0);}

	enum math_ops {
		OP_add,
		OP_substract,
		OP_multiply,
		OP_devide
	};

	enum logic_ops {
		OP_lessthan,
		OP_lessequal,
		OP_equal
	};


	// class callable_virtals_mixin {
	// public:
	// 	virtual ValuePass call(ilang::Arguments &a) RAISE_ERROR;
	// 	//ValuePass call(std::vector<ValuePass> &a); // TODO:
	// };


	// class hashable_virtuals_mixin {
	// public:
	// 	virtual ValuePass get(ilang::Identifier &i) RAISE_ERROR;
	// 	virtual void set(ilang::Identifier &i, ValuePass &v) RAISE_ERROR;

	// };

	template <typename T> class cast_chooser {
		friend class Value_new;
		explicit cast_chooser() {};
	};


  class Value_new// :
		//public math_virtuals_mixin//,
	 //public cast_mixin
	{
		friend class ValuePass;
	public:  // yolo
    union {
      long m_int;
      double m_float;
			bool m_bool;
			char *m_str;
			long m_identifier;
			void *m_ptr;
			char m_sptr[sizeof(std::shared_ptr<void>)];
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
		virtual bool Cast(cast_chooser<bool> c) RAISE_ERROR;
		virtual std::string Cast(cast_chooser<std::string> c) RAISE_ERROR;
		virtual ilang::Function* Cast(cast_chooser<ilang::Function*> c) RAISE_ERROR;
		virtual std::shared_ptr<Hashable> Cast(cast_chooser<Hashable*> c) RAISE_ERROR;
		virtual std::shared_ptr<Class> Cast(cast_chooser<Class*> c) RAISE_ERROR;
		virtual std::shared_ptr<Object> Cast(cast_chooser<Object*> c) RAISE_ERROR;
		virtual std::shared_ptr<Array> Cast(cast_chooser<Array*> c) RAISE_ERROR;

	public:
		template <typename T> auto cast() {
			cast_chooser<T> c;
			return Cast(c);
		}

		// math mixins
	public:
		virtual ValuePass operator + (ValuePass v) RAISE_ERROR;
		virtual ValuePass operator - (ValuePass v) RAISE_ERROR;
		virtual ValuePass operator * (ValuePass v) RAISE_ERROR;
		virtual ValuePass operator / (ValuePass v) RAISE_ERROR;
		virtual ValuePass preform_math_op(math_ops op, long v) RAISE_ERROR;
		virtual ValuePass preform_math_op(math_ops op, double v) RAISE_ERROR;
		virtual ValuePass preform_math_op(math_ops op, std::string v) RAISE_ERROR;

		// logic mixins
	public:
		virtual bool operator == (ValuePass v) RAISE_ERROR;
		virtual bool operator <  (ValuePass v) RAISE_ERROR;
		virtual bool operator <= (ValuePass v) RAISE_ERROR;
		bool operator != (ValuePass v) { return !operator==(v); }
		bool operator > (ValuePass v) { return !operator<=(v); }
		bool operator >= (ValuePass v) { return !operator<(v); }

		virtual bool preform_logic_op(logic_ops op, long v) RAISE_ERROR;
		virtual bool preform_logic_op(logic_ops op, double v) RAISE_ERROR;
		virtual bool preform_logic_op(logic_ops op, std::string) RAISE_ERROR;
		virtual bool preform_logic_op(logic_ops op, bool v) RAISE_ERROR;


		// callable mixins
	public:
		virtual ValuePass call(ilang::Arguments &args) RAISE_ERROR;

		// hashable/array mixin
	public:
		virtual ValuePass get(ValuePass key) RAISE_ERROR;
		virtual ValuePass get(Identifier key) RAISE_ERROR;
		virtual void set(ValuePass key, ValuePass value) RAISE_ERROR;
		virtual void set(Identifier key, ValuePass value) RAISE_ERROR;

		// helpers
	public:
		bool isTrue() { return cast<bool>(); }
		void Print() { std::cout << cast<std::string>(); }

	};

#define VALUE_CAST_CLS_MIXIN(self)																		\
	virtual int Cast(cast_chooser<int> c) { return self; }							\
	virtual long Cast(cast_chooser<long> c) { return self; }						\
	virtual float Cast(cast_chooser<float> c) { return self; }					\
	virtual double Cast(cast_chooser<double> c) { return self; }				\
	virtual bool Cast(cast_chooser<bool> c) { return self != 0; }				\
	virtual std::string Cast(cast_chooser<std::string> c) { std::stringstream ss; ss << self; return ss.str(); }

#define VALUE_MATH_CLS_MIXIN(self)																			\
	virtual ValuePass operator + (ValuePass v) { return v->preform_math_op(OP_add, self); } \
	virtual ValuePass operator - (ValuePass v) { return v->preform_math_op(OP_substract, self); } \
	virtual ValuePass operator * (ValuePass v) { return v->preform_math_op(OP_multiply, self); } \
	virtual ValuePass operator / (ValuePass v) { return v->preform_math_op(OP_devide, self); } \
	virtual ValuePass preform_math_op(math_ops op, long v);								\
	virtual ValuePass preform_math_op(math_ops op, double v);							\
	virtual ValuePass preform_math_op(math_ops op, std::string v);

#define VALUE_LOGIC_CLS_MIXIN(self)																			\
	virtual bool operator < (ValuePass v) { return v->preform_logic_op(OP_lessthan, self); } \
	virtual bool operator <= (ValuePass v) { return v->preform_logic_op(OP_lessequal, self); } \
	virtual bool operator == (ValuePass v) { return v->preform_logic_op(OP_equal, self); } \
	virtual bool preform_logic_op(logic_ops op, long v);									\
	virtual bool preform_logic_op(logic_ops op, double v);								\
	virtual bool preform_logic_op(logic_ops op, std::string v);						\
	virtual bool preform_logic_op(logic_ops op, bool v);

	class IntType : public Value_new {
	public:
		IntType(long i) { m_int = i; }
		virtual const std::type_info &type() { return typeid(long); }

	protected:
		VALUE_CAST_CLS_MIXIN(m_int);

	public:
		VALUE_MATH_CLS_MIXIN(m_int);
		VALUE_LOGIC_CLS_MIXIN(m_int);
	};

	class FloatType : public Value_new {
	public:
		FloatType(double f) { m_float = f; }
		virtual const std::type_info &type() { return typeid(double); }

	protected:
		VALUE_CAST_CLS_MIXIN(m_float);
	public:
		VALUE_MATH_CLS_MIXIN(m_float);
		VALUE_LOGIC_CLS_MIXIN(m_float);
	};

	class BoolType : public Value_new {
	public:
		BoolType(bool b) { m_bool = b; }
		const std::type_info &type() { return typeid(bool); }
	protected:
		VALUE_CAST_CLS_MIXIN(m_bool);
	public:
		VALUE_LOGIC_CLS_MIXIN(m_bool);
	};

	class StringType : public Value_new {
	public:
		StringType(char *str) { m_ptr = new std::string(str); }
		StringType(std::string str) { m_ptr = new std::string(str); }
		StringType(StringType &&s) { m_ptr = s.m_ptr; s.m_ptr = NULL; }
		StringType(StringType const &s) { m_ptr = new std::string(s.GetSelf()); }
		//StringType(std::stringstream str) { m_ptr = new std::string(str.str()); }
		~StringType();// { delete (std::string*)m_ptr; }

		virtual const std::type_info &type() { return typeid(std::string); }
	protected:
		std::string GetSelf() const { return *(std::string*)m_ptr; }
		virtual void copyTo(void *d) {
			new (d) StringType(*this);
		}
		std::string Cast(cast_chooser<std::string> c) {
			return GetSelf();
		}
	public:
		VALUE_MATH_CLS_MIXIN(GetSelf());
		VALUE_LOGIC_CLS_MIXIN(GetSelf());
	};



	// I suppose there is no need for this as there could just be a single function
	// with overloaded parameters

	template<typename Of, typename To, typename... Others> struct _valueMaker { //: _valueMaker<Others...> {
		template <typename T> static inline ValuePass create(T t) {
			return _valueMaker<Others...>::create(t);
		}
		static inline ValuePass create(Of t) {
			//cout << "creating new " << typeid(To).name() << endl;
			return ValuePass(To(t));
		}
		template <typename T> inline ValuePass operator () (T t) {
			return create(t);
		}
	};



}

#endif // _ilang_value
