#ifndef _ilang_function
#define _ilang_function

// TODO: make this functions created using this interface so that the system will have an easily way to create functions that are called rather than having

#include <vector>

//#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "context.h"

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

	// TODO: change this to use std
	typedef boost::function3<void, Context&, ilang::Arguments&, ValuePass*> Function_ptr;

	// TODO: use the new type system to make this easy to work with
	class Arguments {

		std::vector<ilang::ValuePass> pargs;
		std::map<std::string, ilang::ValuePass> kwargs;

		template<typename T> void unwrap(T t) {
			push(valueMaker(t));
		}

		void unwrap(ilang::ValuePass t) {
			push(t);
		}

		template<typename T, typename... types> void unwrap(T t, types... values) {
			unwrap(t);
			unwrap(values...);
		}
		void populate(Context &ctx, Function*);

		friend class Function;
	public:
		void push(ilang::ValuePass);
		void set(std::string, ilang::ValuePass);

		ValuePass get(std::string);
		ValuePass get(int);
		ValuePass operator[](std::string s) { return get(s); }
		ValuePass operator[](int i) { return get(i); }

		std::vector<ilang::ValuePass>::iterator begin() { return pargs.begin(); }
		std::vector<ilang::ValuePass>::iterator end() { return pargs.end(); }


		size_t size();
		//template<typename... types> void inject(types... &values) {}
		// long a; std::string b; args.inject(a, b);

		Arguments();
		Arguments(std::vector<ValuePass> pargs);
		template<typename... types> Arguments(types... values) {
			unwrap(values...);
		}

		template <typename... types> void inject(types & ... values) {
		}
		// int a; std::stinrg b; args.inject(a, b);

	};


	class Function {
	private:
		bool native = false;
		Function_ptr ptr;
		//		ValuePass object_scope = ValuePass();
		ilang::parserNode::Function *func = NULL;
		friend class Arguments;
	public:
		template <typename... types> ilang::ValuePass operator() (types... values) {
			Arguments args(values...);
			Context ctx;
			return call(ctx, args);
		}
		ilang::ValuePass operator() (ilang::Arguments &args) {
			Context ctx;
			return call(ctx, args);
		}
		ilang::ValuePass call(Context&, ilang::Arguments&);

		Function bind(ilang::ValuePass); // bind to an object
		Function bind(Context &ctx);
		Function bind(Hashable*);

		Function(const Function&);
		Function(ilang::parserNode::Function *f, Context &ctx); //, Function_ptr _ptr);
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
		template<typename... Arguments> ilang::Value_Old operator() (Arguments... _args) {
			//ilang::Arguments a;
			//build_arguments(a, args);
			ilang::Arguments args(_args);
			return call(args);
		}
		ilang::Value_Old operator() (ilang::Arguments args) { return call(args); }
		ilang::Value_Old call(ilang::Arguments args) {}
		Function bind(Scope &scope) {}
		Function bindThis(Value_Old) {}
	};
*/

	/*
		class Function {
		Scope *bound_scope;
		public:
		Function (ilang::parserNode::Function *fun);
		Function (Function_ptr fun);
		void BindScope(Scope*);
		ValuePass Call(vector<ilang::Value_Old*> &p);
		};
	*/
} // namespace ilang


#endif // _ilang_function
