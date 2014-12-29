#ifndef _ilang_valuepass
#define _ilang_valuepass

#include "debug.h"

namespace ilang {

	class Value;

	class ValuePass {
  private:
    char m_data[16]; // TODO: don't hard code the size // sizeof(Value) should be 16
	public:
		inline Value *Get() const {
			assert(*(long*)m_data);
			return (Value*)m_data;
		}
    inline Value *operator->() const { return Get(); }
		inline Value &operator*() const { return *Get(); }

    ValuePass() {
			*(long*)m_data = 0;
    }
		ValuePass (Value *v);
		template <typename T>ValuePass (T t) {
			assert(sizeof(T) <= sizeof(m_data));
			assert(dynamic_cast<Value*>(&t));
			new (m_data) T(t);
		}


		// TODO: xvalue constructure
		ValuePass(const ValuePass &x);
		~ValuePass();

		// copy operator
		void operator = (const ValuePass &v);
		void swap(ValuePass &v);

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

	};

}

#endif // _ilang_valuepass
