#ifndef _ilang_internal_debug
#define _ilang_internal_debug

// this should containt all of the interfaces for printing out debuging information

#include <assert.h>
#include <iostream>
//#include "../deps/glog/src/glog/logging.h"

#ifndef NDEBUG
extern int Debug_level;
#define debug(l, x) if(l <= Debug_level) cout << __FILE__<<":"<<__LINE__<<"\t" << x  << ::std::endl;
#define debug_act(x) x
#define debug_break(x)
#else
#define debug(l, x)
#define debug_act(x)
#define debug_not(x) x
#endif

#endif
