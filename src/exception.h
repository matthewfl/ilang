#ifndef _ilang_exception
#define _ilang_exception

#include <exception>

namespace ilang {

	class Exception : public std::exception {
	public:
		virtual const char* name() { return "Exception"; }
		virtual const char* what() const throw() override { return "ilang Exception"; }
	};

	class BadTypeCheck : public Exception {
	public:
		const char* name() override { return "BadTypeCheck"; }
	};

} // namespace ilang


#endif // _ilang_exception
