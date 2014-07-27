#ifndef _ilang_exception
#define _ilang_exception

#include <exception>

namespace ilang {

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

	class BadValueCast : public BadType {
	public:
		const char* name() override { return "BadValueCast"; }
	};

	template <typename T> class BadValueCastType : public BadValueCast {
	};

} // namespace ilang


#endif // _ilang_exception
