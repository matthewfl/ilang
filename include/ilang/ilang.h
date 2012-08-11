#ifndef _ilang_h
#define _ilang_h

// the interface for modules that are compiled

#include "ilang/import.h"
#include "ilang/object.h"

#ifndef ILANG_FILE
#define ILANG_FILE __FILE__
#endif

namespace ilang {
  // used to create wrapper for C++ classes
  // other helpers for wrappers in import.cc
  ValuePass Function_Creater( ValuePass (*fun)(std::vector<ValuePass>&) );
  ValuePass Function_Creater( ValuePass (*fun)(Scope*, std::vector<ValuePass>&) );
  
  template<typename cc> class Class_Creater_class {
    
  };
  template<typename cc> ValuePass Class_Creater() {
    return ValuePass(new ilang::Value(new Class_Creater_class<cc>));
  }
}

#ifndef ILANG_STATIC_LIBRARY
#define ILANG_LIBRARY(x)					\
  extern "C" void ILANG_LOAD (::ilang::ImportScopeC *import)	\
  {								\
    x ;								\
  }



#define ILANG_LIBRARY_NAME(name, x)					\
  ILANG_LIBRARY(x)

//  #warning "ILANG_LIBRARY_NAME, NAME IS NOT USED WHEN NOT STATIC" 


#else // ILANG_STATIC_LIBRARY
#define ILANG_LIBRARY_NAME_REAL(name, x, line)			\ 
namespace { struct _ILANG_STATIC_BIND {				\
  _ILANG_STATIC_BIND () {						\
    ::ilang::ImportScopeC *import = new ImportScopeC(name);		\
    x ;									\
  }} _ILANG_STATIC_BIND_RUN ;				\
}

#define ILANG_LIBRARY_NAME(name, x) ILANG_LIBRARY_NAME_REAL(name, x, __LINE__)

// it appears that an error can not work in this way, but all we need to do is pervent this from compiling which I guess this will do, but it will not be very clear
#define ILANG_LIBRARY(x)
#warning "ILANG_LIBRARY CAN NOT BE USED WHEN STATIC, USE ILANG_LIBRARY_NAME"


#endif // ILANG_STATIC 

//#define ILANG_GLOBAL_PATH(name, x) import->Global(name, x );

//#define ILANG_PATH(name, x) import->Local(name, x );

#define ILANG_CLASS(name, x) import->Set(name, ilang::Class_Creater< x >() );
#define ILANG_REGISTER(name) register(#name, name);

#define ILANG_FUNCTION(name, x) import->Set(name, ilang::Function_Creater( x ) );



#endif // _ilang_h
