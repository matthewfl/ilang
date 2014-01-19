#ifndef _ilang_error
#define _ilang_error

#include <assert.h>
#include <iostream>
#include <string>
#include <sstream>
#include <list>

//#define ILANG_ERROR_TRACE_ENABLED

namespace ilang {

  void error_print_trace();
  class error_trace {
  private:
    friend void error_print_trace();
    std::stringstream message;
  public:
    error_trace();
    error_trace(std::string);
    virtual ~error_trace();
    inline std::stringstream & stream() { return message; }
  };

  extern std::list<error_trace*> ILANG_ERRORS_TRACE;
}

#define error(check, message)					\
  if(!( check )) {						\
  error_print_trace();						\
  std::cerr << "\n\n" << __FILE__ << ":" << __LINE__ << " \t" << message << std::endl << std::flush; \
  exit(1);								\
  }

#define errorTrace(message) \
  error_trace _errors; \
  { _errors.stream() << message ; }

#endif // _ilang_error
