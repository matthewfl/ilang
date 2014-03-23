#ifndef _ilang_function
#define _ilang_function

// TODO: make this functions created using this interface so that the system will have an easily way to create functions that are called rather than having

#include <vector>

//#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "variable.h"
#include "scope.h"
#include "parserTree.h"


namespace ilang {
	//class Scope;
	// First, scope that is running in, arguments passed, return value
	//typedef boost::function<void (Scope*, std::vector<ValuePass>, ValuePass*)> Function_ptr;
	//typedef ValuePass* ValuePass_ptr;
	typedef boost::function3<void, ScopePass, std::vector<ValuePass>, ValuePass*> Function_ptr;

	namespace parserNode {
		class Function;
	}
	class Object;

	struct Function {
		bool native;
		Function_ptr ptr;
		ValuePass object;
		ilang::parserNode::Function *func;
		Function():native(false), object(), func(NULL) {}
		/*inline ValuePass Call (vector<ValuePass &args) {
			ValuePass ret = ValuePass(new ilang::Value);
			if(native)
			ptr( some scope, args, &ret);
			else if(
			}*/
	};

	/* FUTURE INTERFACE
	class Function {
	private:
		// this shiuldn't be here, instead put this into an argument constructor
		template<typename T> inline void build_arguments(ilang::Arguments &a, T t) {
			a.append(t);
		}
		template<typename T, typename... Arguments> inline void build_arguments (ilang::Arguments &a, T t, Arguments... args) {
			a.append(t);
			build_arguments(a, args);
		}
	public:
		template<typename... Arguments> ilang::Value operator() (Arguments... _args) {
			//ilang::Arguments a;
			//build_arguments(a, args);
			ilang::Arguments args(_args);
			return call(args);
		}
		ilang::Value operator() (ilang::Arguments args) { return call(args); }
		ilang::Value call(ilang::Arguments args) {}
		Function bind(Scope &scope) {}
		Function bindThis(Value) {}
	};
*/

	/*
		class Function {
		Scope *bound_scope;
		public:
		Function (ilang::parserNode::Function *fun);
		Function (Function_ptr fun);
		void BindScope(Scope*);
		ValuePass Call(vector<ilang::Value*> &p);
		};
	*/
} // namespace ilang


#endif // _ilang_function
