#ifndef _ilang_h
#define _ilang_h

// the interface for modules that are compiled

#include "ilang/import.h"

#ifndef ILANG_FILE
#define ILANG_FILE __FILE__
#endif


#ifndef ILANG_STATIC
#define ILANG_LIBRARY(x)
#define ILANG_LIBRARY(x)					\
  extern "C" void ILANG_LOAD (::ilang::ImportScopeC *import)	\
  {								\
    x ;								\
  }

#else // ILANG_STATIC
#define ILANG_LIBRARY(x)			\ 
namespace { struct _ILANG_STATIC_BIND {					\
  _LANG_STATIC_BIND () {						\
    ::ilang::ImportScopeC import_(ILANG_FILE);				\
    ::ilang::ImportScopeC *import = &import_;				\
    x ;									\
  }} _ILANG_STATIC_BIND_RUN##__COUNT__## ;				\
}

#endif // ILANG_STATIC 

#define ILANG_GLOBAL_PATH(name, x) import->Global(name, x );

#define ILANG_PATH(name, x) import->Local(name, x );

#define ILANG_CLASS(x) ValuePass(new ilang::Value( ilang::Class_Creater< x >() ))

#define ILANG_FUNCTION(x) ValuePass(new ilang::Value( ilang::Function_Creater( x ) ))

/* example:
   class Curl : public ilang::C_Class {
   public:
   ValuePass Get(std::vector<ValuePass) arguments);
   Curl () {
   register("HttpGet", Get);
   }
   };

   ValuePass Curl_Get(std::vector<ValuePass> arguments);

   ILANG_LIBRARY(
   // for setting things in the global path
   ILANG_GLOBAL_PATH("Net.curl", ILANG_CLASS(Curl))
   ILANG_GLOBAL_PATH("Net.get", ILANG_FUNCTION(Curl_Get))
   // for setting things relative to this file, so if file is called Net.io and is directly in /modules these would be identical
   ILANG_PATH("curl", ILANG_CLASS(Curl))
   ILANG_PATH("get", ILANG_FUNCTION(Curl_Get))
   )

   import Net.curl

   main = {
   net = new(Net.curl);
   net.HttpGet("http://url.com");
   };
*/

#endif // _ilang_h
