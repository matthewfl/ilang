#ifndef _ilang_value
#define _ilang_value

#include <typeinfo>
#include <type_traits>
#include <string>
#include <sstream>
#include <memory>
#include <string.h>

#include <iostream>

#include "debug.h"

#include "valuepass.h"

#include "exception.h"
#include "handle.h"
#include "identifier.h"
#include "context.h"


namespace ilang {

	class Value;
	class Function;
	class Tuple;
	typedef Tuple Arguments;
	class Object;
	class Array;
	class Identifier;
	class Class;
	class Hashable;
	class Tuple;

	typedef Handle<Object> Object_ptr;


#define RAISE_ERROR { throw InvalidOperation(ValuePass(this)); }
#define RAISE_TYPE_ERROR(typ) { throw BadValueCastType<typ>(type().name()); }

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

	template <typename T> class cast_chooser {
		friend class Value;
		explicit cast_chooser() {};
	};

	class Value
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
			char m_sptr[sizeof(Handle<void*>)];
		};

  public:
		virtual ~Value() {}
		virtual const std::type_info &type()=0;

	protected:
		virtual void copyTo(void *d) { memcpy(d, this, sizeof(Value)); }

		// cast mixins
		virtual int Cast(cast_chooser<int> c) RAISE_TYPE_ERROR(int);
		virtual long Cast(cast_chooser<long> c) RAISE_TYPE_ERROR(long);
		virtual float Cast(cast_chooser<float> c) RAISE_TYPE_ERROR(float);
		virtual double Cast(cast_chooser<double> c) RAISE_TYPE_ERROR(double);
		virtual bool Cast(cast_chooser<bool> c) RAISE_TYPE_ERROR(double);
		virtual std::string Cast(cast_chooser<std::string> c) RAISE_TYPE_ERROR(std::string);
		virtual ilang::Function* Cast(cast_chooser<ilang::Function*> c) RAISE_TYPE_ERROR(ilang::Function*);
		virtual Handle<Hashable> Cast(cast_chooser<Hashable*> c) RAISE_TYPE_ERROR(Hashable*);
		virtual Handle<Class> Cast(cast_chooser<Class*> c) RAISE_TYPE_ERROR(Class*);
		virtual Handle<Object> Cast(cast_chooser<Object*> c) RAISE_TYPE_ERROR(Object*);
		virtual Handle<Array> Cast(cast_chooser<Array*> c) RAISE_TYPE_ERROR(Array*);
		virtual Identifier Cast(cast_chooser<Identifier> c) RAISE_TYPE_ERROR(Identifier);
		virtual Handle<Tuple> Cast(cast_chooser<Tuple*> c) RAISE_TYPE_ERROR(Tuple*);

	public:
		template <typename T> auto cast() {
			cast_chooser<T> c;
			return Cast(c);
		}

		void inject(ValuePass &v) {
			ValuePass s(this);
			v.swap(s);
		}

		template <typename T> void inject(Handle<T> &t) {
			auto v = cast<T*>();
			t = v;
		}
		template <typename T> void inject(T &t) {
			auto v = cast<T>();
			inject_helper(t, v);
		}
	private:
		template <typename T> void inject_helper(T* &t, Handle<T> v) {
			t = v.get();
		}
		template <typename T> void inject_helper(T &t, T v) {
			t = v;
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
		virtual ValuePass call(Context &ctx, ilang::Arguments &args) RAISE_ERROR;

		// hashable/array mixin
	public:
		virtual ValuePass get(Context &ctx, ValuePass key) {
			return get(ctx, key->cast<Identifier>());
		}
		virtual ValuePass get(Context &ctx, Identifier key) RAISE_ERROR;
		virtual void set(Context &ctx, ValuePass key, ValuePass value) {
			set(ctx, key->cast<Identifier>(), value);
		}
		virtual void set(Context &ctx, Identifier key, ValuePass value) RAISE_ERROR;

		// helpers
	public:
		bool isTrue() { return cast<bool>(); }
		void Print() { std::cout << cast<std::string>(); }

	};

#define VALUE_CAST_CLS_MIXIN(self)																		\
	virtual int Cast(cast_chooser<int> c) override { return self; }							\
	virtual long Cast(cast_chooser<long> c) override { return self; }						\
	virtual float Cast(cast_chooser<float> c) override { return self; }					\
	virtual double Cast(cast_chooser<double> c) override { return self; }				\
	virtual bool Cast(cast_chooser<bool> c) override { return self != 0; }				\
	virtual std::string Cast(cast_chooser<std::string> c) override { std::stringstream ss; ss << self ; return ss.str(); }

#define VALUE_MATH_CLS_MIXIN(self)																			\
	virtual ValuePass operator + (ValuePass v) override { return v->preform_math_op(OP_add, self); } \
	virtual ValuePass operator - (ValuePass v) override { return v->preform_math_op(OP_substract, self); } \
	virtual ValuePass operator * (ValuePass v) override { return v->preform_math_op(OP_multiply, self); } \
	virtual ValuePass operator / (ValuePass v) override { return v->preform_math_op(OP_devide, self); } \
	virtual ValuePass preform_math_op(math_ops op, long v) override ;								\
	virtual ValuePass preform_math_op(math_ops op, double v) override ;							\
	virtual ValuePass preform_math_op(math_ops op, std::string v) override ;

#define VALUE_LOGIC_CLS_MIXIN(self)																			\
	virtual bool operator < (ValuePass v) override { return v->preform_logic_op(OP_lessthan, self); } \
	virtual bool operator <= (ValuePass v) override { return v->preform_logic_op(OP_lessequal, self); } \
	virtual bool operator == (ValuePass v) override { return v->preform_logic_op(OP_equal, self); } \
	virtual bool preform_logic_op(logic_ops op, long v) override ;				\
	virtual bool preform_logic_op(logic_ops op, double v) override ;			\
	virtual bool preform_logic_op(logic_ops op, std::string v) override ;	\
	virtual bool preform_logic_op(logic_ops op, bool v) override ;

	class IntType : public Value {
	public:
		IntType(long i) { m_int = i; }
		virtual const std::type_info &type() { return typeid(long); }

	protected:
		VALUE_CAST_CLS_MIXIN(m_int);

	public:
		VALUE_MATH_CLS_MIXIN(m_int);
		VALUE_LOGIC_CLS_MIXIN(m_int);
		Identifier Cast(cast_chooser<Identifier> c) { return Identifier(m_int); }
	};

	class FloatType : public Value {
	public:
		FloatType(double f) { m_float = f; }
		virtual const std::type_info &type() { return typeid(double); }

	protected:
		VALUE_CAST_CLS_MIXIN(m_float);
	public:
		VALUE_MATH_CLS_MIXIN(m_float);
		VALUE_LOGIC_CLS_MIXIN(m_float);
	};

	class BoolType : public Value {
	public:
		BoolType(bool b) { m_bool = b; }
		const std::type_info &type() { return typeid(bool); }
	protected:
		VALUE_CAST_CLS_MIXIN(m_bool);
	public:
		VALUE_LOGIC_CLS_MIXIN(m_bool);
	};

	class StringType : public Value {
	public:
		StringType(const char *str) { m_ptr = new std::string(str); }
		StringType(std::string str) { m_ptr = new std::string(str); }
		StringType(StringType &&s) { m_ptr = s.m_ptr; s.m_ptr = NULL; }
		StringType(StringType const &s) { m_ptr = new std::string(s.GetSelf()); }
		StringType(Identifier const &i) { m_ptr = new std::string(i.str()); }
		//StringType(std::stringstream str) { m_ptr = new std::string(str.str()); }
		~StringType();// { delete (std::string*)m_ptr; }

		virtual const std::type_info &type() { return typeid(std::string); }
	protected:
		std::string GetSelf() const { return *(std::string*)m_ptr; }
		virtual void copyTo(void *d) {
			new (d) StringType(*this);
		}
		std::string Cast(cast_chooser<std::string> c) override {
			return GetSelf();
		}
		Identifier Cast(cast_chooser<Identifier> c) override {
			return Identifier(GetSelf());
		}
	public:
		VALUE_MATH_CLS_MIXIN(GetSelf());
		VALUE_LOGIC_CLS_MIXIN(GetSelf());
	};


	// I suppose there is no need for this as there could just be a single function
	// with overloaded parameters

	template<typename Of, typename To, typename... Others> struct _valueMaker {
		template <typename T> static inline ValuePass create(T t) {
			return _valueMaker<Others...>::create(t);
		}
		static inline ValuePass create(Of t) {
			return ValuePass(To(t));
		}

		template <typename T> inline ValuePass operator () (T t) {
			return create(t);
		}
	};



}

#endif // _ilang_value
