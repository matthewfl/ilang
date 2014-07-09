#ifndef _ilang_helpers
#define _ilang_helpers

#include <list>
#include <vector>
#include <algorithm>


// TODO: should create a derived class of vector with these helpers

namespace ilang {
	// convert lists and other types to vector while
	// transistioning to vectors for list types
	template <typename T> auto toVector(std::list<T> l) {
		return std::vector<T>(l.begin(), l.end());
	}

	// combine the elements in two different vectors
	template<typename T> std::vector<T> unionVectors(std::vector<T> ret) {
		std::sort(ret.begin(), ret.end());
		std::unique(ret.begin(), ret.end());
		return ret;
	}
	template<typename T, typename... others> std::vector<T> unionVectors(std::vector<T> a, others... oth) {
		std::vector<T> ret;
		std::vector<T> b = unionVectors(oth...);
		std::set_union(a.begin(), a.end(), b.begin(), b.end(), ret);
		return ret;
	}

	template<typename T> void vectorRemove(std::vector<T> &v, T item) {
		auto it = std::find(v.begin(), v.end(), item);
		if(it != v.end())
			v.erase(it);
	}

	template <typename T> std::unordered_set<T> unionSets(std::unordered_set<T> a) {
		return a;
	}
	template <typename T, typename... others> std::unordered_set<T> unionSets(std::unordered_set<T> a, others... oth) {
		std::unordered_set<T> ret = unionSets(oth...);
		ret.insert(a.begin(), a.end());
		return ret;
	}


}

#endif // _ilang_helpers
