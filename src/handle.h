#ifndef _ilang_handle
#define _ilang_handle

#include <memory>
#include <boost/smart_ptr.hpp>
#include <atomic>
#include <assert.h>


namespace ilang {
	// using std::make_handle;
	// using Handle;
	// using std::dynamic_pointer_cast;

	// template <typename T> using Handle = Handle<T>;

	//template <typename T> using make_handle = make_handle<T>;
	template <typename T> using Handle = boost::intrusive_ptr<T>;
	using boost::dynamic_pointer_cast;
	using boost::static_pointer_cast;

	template <typename T, typename... args> Handle<T> make_handle(args ... arg) {
		return Handle<T>(new T(arg...));
	}

	class Handle_base {
	private:
		mutable std::atomic<unsigned int> ptr_count;
		friend void handle_ptr_add_ref(const Handle_base *);
		friend bool handle_ptr_release(const Handle_base *);
	public:
		Handle_base() { ptr_count = 0; }
	};

	inline void handle_ptr_add_ref(const Handle_base * p) {
		p->ptr_count++;
	}
	inline bool handle_ptr_release(const Handle_base * p) {
		assert(p->ptr_count != 0);
		return --p->ptr_count;
	}
	template <typename T> inline void intrusive_ptr_add_ref(const T *p) {
		handle_ptr_add_ref(p);
	}
	template <typename T> inline void intrusive_ptr_release(const T *p) {
		if(!handle_ptr_release(p)) {
			delete p;
		}
	}
}

#endif // _ilang_handle
