#ifndef _ilang_value
#define _ilang_value

#include <typeinfo>
#include <type_traits>
#include <string>
#include "debug.h"

#include <iostream>
using namespace std;

namespace ilang {

	class ValuePass_new;

	// template <typename T> class convert_to_mixin {
	// public:
	// 	void cast(T &v) { get(); }
	// };

#define RAISE_ERROR {assert(0);}

	class math_virtuals_mixin {
	public:
		virtual ValuePass_new && operator + (ValuePass_new &v) RAISE_ERROR;
		virtual ValuePass_new && operator - (ValuePass_new &v) RAISE_ERROR;
		virtual ValuePass_new && operator * (ValuePass_new &v) RAISE_ERROR;
		virtual ValuePass_new && operator / (ValuePass_new &v) RAISE_ERROR;
		virtual ValuePass_new && preform_math_op(ValuePass_new &v) RAISE_ERROR;
	};


	class Arguments;
	class callable_virtals_mixin {
	public:
		virtual ValuePass_new && call(ilang::Arguments &a) RAISE_ERROR;
	};

	class Identifier;
	class hashable_virtuals_mixin {
	public:
		virtual ValuePass_new && get(ilang::Identifier &i) RAISE_ERROR;
		virtual void set(ilang::Identifier &i, ValuePass_new &v) RAISE_ERROR;
	};

	template <typename T> class cast_mixin_base {
	public:
		//virtual void cast(T &t) RAISE_ERROR;
	};

	class cast_mixin :
		public cast_mixin_base<int>,
		public cast_mixin_base<long>,
		public cast_mixin_base<float>,
		public cast_mixin_base<double>//,
		//public cast_mixin_base<bool>
	{
	public:
		virtual void cast(int &i) RAISE_ERROR;
	};


  class Value_new :
		public math_virtuals_mixin,
		public cast_mixin
	{
	public:  // yolo
    union {
      long m_int;
      double m_float;
			bool m_bool;
			char *m_str;
			long m_identifier;
		};

  public:
    Value_new() {
    }
		virtual ~Value_new() {}

		virtual const std::type_info &type() { return typeid(void); }
  };

  class ValuePass_new {
  private:
    char m_data[sizeof(Value_new)];
  public:
    Value_new *Get();
		// {
    //   return (Value_new*)m_data;
    // }
    operator Value_new() {
      return *Get();
    }
    Value_new *operator->() {
      return Get(); //(Value_new*)m_data;
    }

    ValuePass_new() {
			new (m_data) Value_new;
    }
		// ValuePass_new (const Value_new &v) {
		// 	new (m_data) Value_new;
		// 	*(Value_new*)m_data = v;
		// }
		template <typename T>ValuePass_new (T t) {
			assert(sizeof(T) == sizeof(Value_new));
			//assert(dynamic_cast<Value*>(&t));
			cout << "here\n";
			new (m_data) T(t);
			//*(Value*)m_data = t;
		}
		// TODO: going to need support to tracking with something is coppied
		//ValuePass_new (const ValuePass_new &v) {
			//			new (m_data) Value_new;
		//	struct a { char a[sizeof(Value_new)]; };
		//	(a)m_data = (a)v.m_data;
		//}

		~ValuePass_new() {
			((Value_new*)m_data)->~Value_new();
		}
  };

	enum math_ops {
		OP_add,
		OP_substract,
		OP_multiply,
		OP_devide
	};

	template <typename Self, typename Other> class math_core_mixin {
	protected:
		virtual Self GetRaw(Self)=0;
		virtual ValuePass_new && preform_math_op(math_ops op, Other val) {
			switch(op) {
			case OP_add:
				return valueMaker(GetRaw(Self()) + val);
			case OP_substract:
				return valueMaker(GetRaw(Self()) - val);
			case OP_multiply:
				return valueMaker(GetRaw(Self()) * val);
			case OP_devide:
				return valueMaker(GetRaw(Self()) / val);
			}
		}
	};


	template <typename Self> class math_mixins :
		protected math_core_mixin<Self, int>,
		protected math_core_mixin<Self, long>,
		protected math_core_mixin<Self, float>,
		protected math_core_mixin<Self, double> {
	public:

		virtual ValuePass_new && operator + (ValuePass_new &v) {
			return v->preform_math_op(OP_add, GetRaw(Self()));
		}
		//virtual preform_math_op(math_ops op,

	};

  // template <typename Of, typename To, typename... Others> struct _valueMaker {
	// 	// template <int same> struct create {
	// 	// 	template <typename T> Value &&c(T t) {}
	// 	// };
	// 	// template <> struct create <true> {
	// 	// 	template <typename T> Value &&c(T t) {
	// 	// 		return To(t);
	// 	// 	}
	// 	// };
	// 	// template <> struct create<false> {
	// 	// 	template <typename T> Value &&c(T t) {
	// 	// 		return _valueMaker<Others...>::operator()(t);
	// 	// 	}
	// 	// };
  //   // template <bool same, typename T> Value &&create(T t) {};
	// 	// template <typename T> Value &&create<true, T>(T t) {
	// 	// 	return To(t);
	// 	// }
	// 	// template<typename T> Value &&create<false, T>(T t) {
	// 	// 	return _valueMaker<Others...>::operator() (t);
	// 	// }
	// 	template<typename T, bool asdf> Value &&create(T t) {

	// 	}

	// 	template <typename T> static Value && c (T t) {
	// 		return create<T, std::is_same<Of, T>::value>(t);
	// 		//return create<std::is_same<Of, T>::value, T>(t);
	// 	}
  // };

  // template <> struct _valueMaker<void, void> {
	// 	Value &&create
  //   template <typename T> static Value && operator () (T t) {
  //     // base case, should have an assert here or something
	// 		assert(0);
  //   }
  // };

	template<typename Of, typename To, typename... Others> struct _valueMaker {
		template <typename T> static ValuePass_new && create(T t) {
			return _valueMaker<Others...>::create(t);
		}
		static ValuePass_new && create(Of t) {
			cout << "creating new " << typeid(To).name() << endl;
			return ValuePass_new(To(t));
		}
		template <typename T> inline ValuePass_new && operator () (T t) {
			return create(t);
		}
	};
	// template<> struct _valueMaker <void, void> {
	// 	template <typename T, bool same> static Value &&creater(T t) {}
	// 	template <typename T> static Value &&create(T t) {
	// 		assert(0);
	// 	}
	// };

	class IntType : protected Value_new {
	public:
		IntType(int i) { m_int = i; }
		IntType(long i) { m_int = i; }
		ValuePass_new && operator + (ValuePass_new &v) {
			return IntType(m_int + v->m_int);
		}
		~IntType() {}
		virtual void cast(int &i) {
			i = m_int;
		}
		virtual void cast(long &i) {
			i = m_int;
		}

		//virtual const std::type_info &type() { return typeid(void); }
		virtual const std::type_info &type() { return typeid(long); }
	};

	class FloatType : protected Value_new {
	public:
		FloatType(double f) { m_float = f; }
		ValuePass_new && operator + (ValuePass_new &v) {
			return FloatType(m_float + v->m_float);
		}
		void cast(float &f) {
			f = m_float;
		}
		void cast(double &f) {
			f = m_float;
		}
	};

	class BoolType : protected Value_new {
	public:
		BoolType(bool b) { m_bool = b; }
		void cast(bool &b) {
			b = m_bool;
		}
	};

	class StringType : protected Value_new {
	public:
		StringType(char *str) {}
		StringType(std::string str) {}
	};


	// I suppose there is no need for this as there could just be a single function
	// with overloaded parameters
	static auto valueMaker = _valueMaker<
		int, IntType,
		long, IntType,
		double, FloatType,
		float, FloatType,
		bool, BoolType,
		char*, StringType,
		std::string, StringType
		>();

}

#endif // _ilang_value
