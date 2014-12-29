

template <typename T> class Cache {
private:
	bool set = false;
	T val;
public:
	T & operator() (T &&t) {
		set = true;
		val = t;
		return val;
	}
	T & operator() () {
		return val;
	}
	operator bool() { return set; }
	operator T &() { return t; }
};

// use

class Function {
	// ....
	Cache<IdentifierSet> cache;
};

Function::UndefinedElements() {
	if(cache) return cache;
	// ...
	return cache(valueMaker(123));
}
