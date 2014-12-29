#ifndef _ilang_exception
#define _ilang_exception

#include <exception>

#include "valuepass.h"

namespace ilang {

	class Value;

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

	class InvalidOperation : public Exception {
	public:
		virtual const char* what() const throw() override { return "ilang invalid operation"; }
		ValuePass of;
		InvalidOperation() {}
		InvalidOperation(ValuePass f) : of(f) {}
	};

} // namespace ilang


#endif // _ilang_exception
