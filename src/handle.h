#ifndef _ilang_handle
#define _ilang_handle

#include <memory>
#include <boost/intrusive_ptr.hpp>
#include <atomic>

namespace ilang {
	// using std::make_handle;
	// using Handle;
	// using std::dynamic_pointer_cast;

	// template <typename T> using Handle = Handle<T>;

	template <typename T> using Handle = boost::intrusive_ptr<T>;
	using boost::dynamic_pointer_cast;
	template <typename T, typename... args> Handle<T> make_handle(args... arg) {
		return Handle<T>(new T(arg...));
	}

	//template <typename T> using make_handle = make_handle<T>;

	class Handle_counter {
	private:
		std::atomic<unsigned int> ptr_count;//(0); // hopefully this zeros it out, which it seems to imply
		template <typename T> friend void intrusive_ptr_add_ref(T*);
		template <typename T> friend void intrusive_ptr_release(T*);
	};
	template <typename T> void //std::enable_if<std::is_base_of<Handle_counter, T>::value, void>::type
	intrusive_ptr_add_ref(T * p) {
		//static_cast<Handle_counter*>(p)->ptr_count++;
	}
	template <typename T> void //std::enable_if<std::is_base_of<Handle_counter, T>::value, void>::type
	intrusive_ptr_release(T * p) {
		//if(--(static_cast<Handle_counter*>(p)->ptr_count) == 0)
			delete p;
	}
}

#endif // _ilang_handle
