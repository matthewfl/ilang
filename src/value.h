#ifndef _ilang_value
#define _ilang_value

#include <type_traits>
#include "debug.h"

namespace ilang {

  class Value  {
	protected:
    union {
      long m_int;
      double m_float;
    };

  public:
    Value() {
    }

		// operations
		virtual Value && operator + (Value &v) {}
		virtual Value && operator - (Value &v) {}
		virtual Value && operator * (Value &v) {}
		virtual Value && operator / (Value &v) {}
  };

  class ValuePass {
  private:
    char m_data[sizeof(Value)];
  public:
    Value &Get() {
      return *(Value*)m_data;
    }
    operator Value() {
      return Get();
    }
    Value *operator->() {
      return (Value*)m_data;
    }

    ValuePass() {
      new (m_data) Value;
    }

  };



  template <typename Of, typename To, typename... Others> struct _valueMaker {
    template <typename T> Value && operator () (T &t) {
      if(std::is_same<Of, T>::value) {
        return To(t);
      }
      return _valueMaker<Others...>::operator() (t);
    }
  };

  template <> struct _valueMaker<void, void> {
    template <typename T> Value && operator () (T &t) {
      // base case, should have an assert here or something
			assert(0);
    }
  };

	class IntType : protected Value {
	public:
		IntType(int i) { m_int = i; }
		IntType(long i) { m_int = i; }
		Value && operator + (Value &v) {
			return IntType(m_int + 5);
		}
	};

	class FloatType : protected Value {
	public:
		FloatType(double f) { m_float = f; }
	};


	const auto valueMaker = _valueMaker<
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
