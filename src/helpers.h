#ifndef _ilang_helpers
#define _ilang_helpers

#include <list>
#include <vector>

namespace ilang {
	// convert lists and other types to vector while
	// transistioning to vectors for list types
	template <typename T> auto toVector(std::list<T> l) {
		return std::vector<T>(l.begin(), l.end());
	}
}

#endif // _ilang_helpers
