#ifndef _ilang_exception
#define _ilang_exception

#include <exception>

namespace ilang {

	class Value_new;

	class Exception : public std::exception {
	public:
		virtual const char* name() { return "Exception"; }
		virtual const char* what() const throw() override { return "ilang Exception"; }
	};

	class BadType : public Exception {};

	class BadTypeCheck : public BadType {
	public:
		const char* name() override { return "BadTypeCheck"; }
	};

	// this is more of an internal error...?
	class BadValueCast : public BadType {
	public:
		const char* name() override { return "BadValueCast"; }
		BadValueCast(char *f) : from(f) {}
		char *from;
	};

	template <typename T> class BadValueCastType : public BadValueCast {
	public:
		BadValueCastType(const char *f) : BadValueCast(const_cast<char*>(f)) {}
	};

} // namespace ilang


#endif // _ilang_exception
