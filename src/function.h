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

	struct Function_old {
		bool native;
		Function_ptr ptr;
		ValuePass object;
		ilang::parserNode::Function *func;
		Function_old():native(false), object(), func(NULL) {}
		/*inline ValuePass Call (vector<ValuePass &args) {
			ValuePass ret = ValuePass(new ilang::Value);
			if(native)
			ptr( some scope, args, &ret);
			else if(
			}*/
	};

	class Function;

	class Arguments {
		friend class Funciton;
		std::vector<ilang::ValuePass> pargs;
		std::map<std::string, ilang::ValuePass> kwargs;

		template<typename T> void unwrap(T t) {
			push(ilang::ValuePass(new ilang::Value(t)));
		}

		template<typename T, typename... types> void unwrap(T t, types... values) {
			unwrap(t);
			unwrap(values...);
		}
		void populate(ScopePass, Function*);
	public:
		void push(ilang::ValuePass);
		void set(std::string, ilang::ValuePass);
		Arguments();
		// TODO: use the new type system to make this easy to work with
		Arguments(std::vector<ValuePass> pargs);
		template<typename... types> Arguments(types... values) : Arguments() {
			unwrap(values...);
		}
	};


	class Function {
	private:
		bool native;
		Function_ptr ptr;
		ScopePass contained_scope;
		ValuePass object_scope;
		ilang::parserNode::Function *func;
	public:
		template <typename... types> ilang::ValuePass operator() (types... values) {
			Arguments args(values...);
			return call(args);
		}
		ilang::ValuePass operator() (ilang::Arguments &args) { return call(args); }
		ilang::ValuePass call(ilang::Arguments&);
		Function bind(ilang::ValuePass); // bind to an object

		Function(const Function&);
		Function(ilang::parserNode::Function *f, ScopePass scope, Function_ptr _ptr);
		Function(Function_ptr _ptr);
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
