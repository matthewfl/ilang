#ifndef _ilang_handle
#define _ilang_handle

#include <memory>

namespace ilang {
	using std::shared_ptr;
	using std::make_shared;
	using std::dynamic_pointer_cast;

	template <typename T> using Handle = std::shared_ptr<T>;
}

#endif // _ilang_handle
