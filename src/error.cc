#include "error.h"

/* A error should be used to report an error to a user
 * the trace should as much as possible be useful to finding an error in the .i files
 * assert() should be used for problems with the code itself
 */

using namespace std;

namespace ilang {
  std::list<error_trace*> ILANG_ERRORS_TRACE;

  void error_print_trace() {
    cerr << "\33[0;31milang internal trace:\n";
#ifdef ILANG_ERROR_TRACE_ENABLED
    for(auto it : ILANG_ERRORS_TRACE) {
      cerr << it->message.str() << endl;
    }
#else
    cerr << "Error trace not enabled\n";
#endif
    cerr << "\33[0m";
  }
  error_trace::error_trace() {
#ifdef ILANG_ERROR_TRACE_ENABLED
    ILANG_ERRORS_TRACE.push_back(this);
#endif
  }
  error_trace::error_trace(std::string ss) {
#ifdef ILANG_ERROR_TRACE_ENABLED
    ILANG_ERRORS_TRACE.push_back(this);
    message << ss;
#endif
  }
  error_trace::~error_trace() {
#ifdef ILANG_ERROR_TRACE_ENABLED
    error(ILANG_ERRORS_TRACE.back() == this,
	  "Poping of error_taces not coming off in the same order");

    ILANG_ERRORS_TRACE.pop_back();
#endif
  }
}
