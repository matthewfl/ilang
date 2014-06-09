#ifndef _ilang_value
#define _ilang_value

#include <type_traits>
#include "debug.h"

namespace ilang {

  class Value_new  {
	public:  // yolo
    union {
      long m_int;
      double m_float;
    };

  public:
    Value_new() {
    }
		virtual ~Value_new() {}

		// operations
		virtual Value_new && operator + (Value_new &v) {}
		virtual Value_new && operator - (Value_new &v) {}
		virtual Value_new && operator * (Value_new &v) {}
		virtual Value_new && operator / (Value_new &v) {}
  };

  class ValuePass_new {
  private:
    char m_data[sizeof(Value_new)];
  public:
    Value_new *Get() {
      return (Value_new*)m_data;
    }
    operator Value_new() {
      return *Get();
    }
    Value_new *operator->() {
      return (Value_new*)m_data;
    }

    ValuePass_new() {
      new (m_data) Value_new;
    }
		ValuePass_new (const Value_new &v) {
			new (m_data) Value_new;
			*(Value_new*)m_data = v;
		}
		template <typename T>ValuePass_new (T t) {
			assert(sizeof(T) == sizeof(Value_new));
			//assert(dynamic_cast<Value*>(&t));
			new (m_data) T(t);
			//*(Value*)m_data = t;
		}
		ValuePass_new (const ValuePass_new &v) {
			new (m_data) Value_new;
			*(Value_new*)m_data = *(Value_new*)v.m_data;
		}

		~ValuePass_new() {
			((Value_new*)m_data)->~Value_new();
		}
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
		Value_new && operator + (Value_new &v) {
			return IntType(m_int + v.m_int);
		}
		~IntType() {}
	};

	class FloatType : protected Value_new {
	public:
		FloatType(double f) { m_float = f; }
		Value_new && operator + (Value_new &v) {
			return FloatType(m_float + v.m_float);
		}
	};


	static auto valueMaker = _valueMaker<
		int, IntType,
		long, IntType,
		double, FloatType,
		float, FloatType

		// bool, BoolType,
		// char*, StringType,
		// std::string, StringType
		>();

}

#endif // _ilang_value
