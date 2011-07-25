#ifndef _ilang_internal_debug
#define _ilang_internal_debug

// this should containt all of the interfaces for printing out debuging information

#include <assert.h>
#include <iostream>

#ifndef NDEBUG
#define debug(x) ::std::cout << __FILE__<<":"<<__LINE__<<"\t" << x  << ::std::endl;
#define debug_act(x) x
#define debug_break(x)
#else
#define debug(x)
#define debug_act(x)
#define debug_break(x) return x;
#endif

#endif
