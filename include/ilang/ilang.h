#ifndef _ilang_h
#define _ilang_h

// the interface for modules that are compiled

#include "ilang/import.h"

#ifndef ILANG_FILE
#define ILANG_FILE __FILE__
#endif


#ifndef ILANG_STATIC
#define ILANG_LIBRARY(x)					\
  extern "C" void ILANG_LOAD (::ilang::ImportScopeC *import)	\
  {								\
    x ;								\
  }

#define ILANG_LIBRARY_NAME(name, x)					\
  ILANG_LIBRARY(x)

//  #warning "ILANG_LIBRARY_NAME, NAME IS NOT USED WHEN NOT STATIC" 


#else // ILANG_STATIC
#define ILANG_LIBRARY_NAME(name, x)			\ 
namespace { struct _ILANG_STATIC_BIND {					\
  _LANG_STATIC_BIND () {						\
    ::ilang::ImportScopeC import_(ILANG_FILE);				\
    ::ilang::ImportScopeC *import = &import_;				\
    x ;									\
  }} _ILANG_STATIC_BIND_RUN##__COUNT__## ;				\
}

// it appears that an error can not work in this way, but all we need to do is pervent this from compiling which I guess this will do, but it will not be very clear
#define ILANG_LIBRARY(x)						\
  #error "ILANG_LIBRARY CAN NOT BE USED WHEN STATIC, USE ILANG_LIBRARY_NAME"


#endif // ILANG_STATIC 

//#define ILANG_GLOBAL_PATH(name, x) import->Global(name, x );

//#define ILANG_PATH(name, x) import->Local(name, x );

#define ILANG_CLASS(name, x) import->Set(name, ValuePass(new ilang::Value( ilang::Class_Creater< x >() )) );

#define ILANG_FUNCTION(name, x) import->Set(name, ilang::Function_Creater( x ) );



#endif // _ilang_h
