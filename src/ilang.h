#ifndef _ilang_h
#define _ilang_h

// the interface for modules that are compiled

#include "import.h"
#include "object.h"
#include "function.h"
#include <string>

#include <iostream>

#ifndef ILANG_FILE
#define ILANG_FILE __FILE__
#endif

/*
	namespace ilang {
	// a very bad hack, I would like to find a better way
	namespace {
	ilang::ValuePass Function_Creater( ilang::ValuePass (*fun)(std::vector<ilang::ValuePass>&) ) {
	ilang::Function_ptr f = [fun](ilang::Scope *scope, std::vector<ilang::ValuePass> & args, ilang::ValuePass *ret) {
	*ret = (*fun)(args);
	};
	return ValuePass(new ilang::Value(f));
	}
	ilang::ValuePass Function_Creater( ilang::ValuePass (*fun)(ilang::Scope*, std::vector<ilang::ValuePass>&) ) {
	ilang::Function_ptr f = [fun](ilang::Scope *scope, std::vector<ilang::ValuePass> & args, ilang::ValuePass *ret) {
	*ret = (*fun)(scope, args);
	};
	return ValuePass(new ilang::Value(f));
	}
	}
	}
*/



namespace ilang {
	// used to create wrapper for C++ classes
	// other helpers for wrappers in import.cc
	ValuePass Function_Creater( ValuePass (*fun)(Arguments&) );
	ValuePass Function_Creater( ValuePass (*fun)(Scope*, Arguments&) );
	ValuePass Function_Creater( ValuePass (*fun)(ScopePass, Arguments&) );

	class C_Class {
	private:
		std::map<std::string, ilang::Variable*> m_members;
	public:
		C_Class() {}
		virtual ~C_Class();
		template <typename cla> void reg(std::string name, ValuePass (cla::*fun)(ilang::Arguments &args) ) {
			assert(m_members.find(name) == m_members.end());
			cla *self = (cla*)this;
			//assert(self);
			ilang::Function f([fun, self](ScopePass scope, ilang::Arguments &args, ValuePass *ret) {
					*ret = (self ->* fun)(args);
					assert(*ret);
				});
			std::list<std::string> mod = {"Const"};
			ilang::Variable *var = new ilang::Variable(name, mod);
			assert(0); // TODO:
			//var->Set(valueMaker(f));
			m_members.insert(std::pair<std::string, ilang::Variable*>(name, var));
		}
		template <typename cla> void reg(std::string name, ValuePass (cla::*fun)(Scope *s, ilang::Arguments &args) ) {
			assert(m_members.find(name) == m_members.end());
			cla *self = (cla*)this;
			//assert(self);
			ilang::Function f([fun, self](ScopePass scope, ilang::Arguments &args, ValuePass *ret) {
					*ret = (self ->* fun)(scope.get(), args);
					assert(*ret);
				});
			std::list<std::string> mod = {"Const"};
			ilang::Variable *var = new ilang::Variable(name, mod);
			assert(0); // TODO:
			//var->Set(valueMaker(f));
			m_members.insert(std::pair<std::string, ilang::Variable*>(name, var));
		}
		template <typename cla> void reg(std::string name, ValuePass (cla::*fun)(ScopePass s, ilang::Arguments &args) ) {
			assert(m_members.find(name) == m_members.end());
			cla *self = (cla*)this;
			//assert(self);
			ilang::Function f([fun, self](ScopePass scope, ilang::Arguments &args, ValuePass *ret) {
					*ret = (self ->* fun)(scope, args);
					assert(*ret);
				});
			std::list<std::string> mod = {"Const"};
			ilang::Variable *var = new ilang::Variable(name, mod);
			assert(0); // TODO:
			//var->Set(valueMaker(f));
			m_members.insert(std::pair<std::string, ilang::Variable*>(name, var));
		}
		ilang::Variable* operator[](std::string name) {
			auto it = m_members.find(name);
			if(it != m_members.end()) return it->second;
			return NULL;
		}
	};

	template<typename cc> class Class_Creater_class : public Class {
	public:
		Class_Creater_class () : Class(NULL, NULL, ScopePass()) {}
		Object * NewClass (ValuePass self) {
			return new Object(new cc);
		}
	};
	template<typename cc> ValuePass Class_Creater() {
		auto c = make_shared<Class_Creater_class<cc> >();
		return valueMaker(dynamic_pointer_cast<Class>(c));
		//ilang::Class *c = new Class_Creater_class<cc>();
		//return ValuePass(new ilang::Value_Old(c));
	}
}

#ifndef ILANG_STATIC_LIBRARY
#define ILANG_LIBRARY(x)																			\
	extern "C" void ILANG_LOAD (::ilang::ImportScopeC *import)	\
	{																														\
		x ;																												\
	}



#define ILANG_LIBRARY_NAME(name, x)							\
	ILANG_LIBRARY(x)
#define ILANG_LIBRARY_NAME_REAL(name, uid, x)		\
	ILANG_LIBRARY(x)

//	#warning "ILANG_LIBRARY_NAME, NAME IS NOT USED WHEN NOT STATIC"


#else // ILANG_STATIC_LIBRARY
#define ILANG_LIBRARY_NAME_REAL(name, uid, x)														\
	namespace { struct _ILANG_STATIC_BIND##uid {													\
			_ILANG_STATIC_BIND##uid () {																			\
				::ilang::ImportScopeC *import = new ::ilang::ImportScopeC(name); \
				x ;																															\
			}} _ILANG_STATIC_BIND_RUN##uid ;																	\
	}

#define ILANG_LIBRARY_NAME(name, x) ILANG_LIBRARY_NAME_REAL(name, __, x)

// it appears that an error can not work in this way, but all we need to do is pervent this from compiling which I guess this will do, but it will not be very clear
// if there is no macro then this should cause an error and stop
//#define ILANG_LIBRARY(x)
//#warning "ILANG_LIBRARY CAN NOT BE USED WHEN STATIC, USE ILANG_LIBRARY_NAME"


#endif // ILANG_STATIC

//#define ILANG_GLOBAL_PATH(name, x) import->Global(name, x );

//#define ILANG_PATH(name, x) import->Local(name, x );

#define ILANG_CLASS(name, x) import->Set(name, ilang::Class_Creater< x >() );
#define ILANG_REGISTER(name) register(#name, name);

#define ILANG_FUNCTION(name, x) import->Set(name, ilang::Function_Creater( x ) );



#endif // _ilang_h
