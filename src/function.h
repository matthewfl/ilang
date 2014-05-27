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
	class Function;
	class Arguments;
	class Object;


	namespace parserNode {
		class Function;
	}

	typedef boost::function3<void, ScopePass, std::vector<ValuePass>, ValuePass*> Function_ptr;

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

	class Arguments {

		std::vector<ilang::ValuePass> pargs;
		std::map<std::string, ilang::ValuePass> kwargs;

		template<typename T> void unwrap(T t) {
			push(ilang::ValuePass(new ilang::Value(t)));
		}

		void unwrap(ilang::ValuePass t) {
			push(t);
		}

		template<typename T, typename... types> void unwrap(T t, types... values) {
			unwrap(t);
			unwrap(values...);
		}
		void populate(ScopePass, Function*);

		friend class Function;
	public:
		void push(ilang::ValuePass);
		void set(std::string, ilang::ValuePass);
		ValuePass get(std::string);
		ValuePass get(int);
		ValuePass operator[](std::string s) { return get(s); }
		ValuePass operator[](int i) { return get(i); }
		Arguments();
		// TODO: use the new type system to make this easy to work with
		Arguments(std::vector<ValuePass> pargs);
		template<typename... types> Arguments(types... values) : Arguments() {
			unwrap(values...);
		}

	};


	class Function {
	private:
		bool native = false;
		Function_ptr ptr;
		ScopePass contained_scope = ScopePass();
		ValuePass object_scope = ValuePass();
		ilang::parserNode::Function *func = NULL;
		friend class Arguments;
	public:
		template <typename... types> ilang::ValuePass operator() (types... values) {
			Arguments args(values...);
			return call(ScopePass(), args);
		}
		ilang::ValuePass operator() (ilang::Arguments &args) { return call(ScopePass(), args); }
		ilang::ValuePass call(ScopePass, ilang::Arguments&);
		Function bind(ilang::ValuePass); // bind to an object
		Function capture(ScopePass);

		Function(const Function&);
		Function(ilang::parserNode::Function *f, ScopePass scope, Function_ptr _ptr);
		Function(Function_ptr _ptr);
		Function();
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
