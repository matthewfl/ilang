#ifndef _ilang_function
#define _ilang_function

// TODO: make this functions created using this interface so that the system will have an easily way to create functions that are called rather than having

#include <vector>

//#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "context.h"

#include "variable_new.h"
#include "scope_new.h"
#include "parserTree.h"
#include "hashable.h"


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

	// TODO: refactor this into being a tuple system, which can then be reused elsewhere
	class Arguments : public Hashable {

		// std::vector<ilang::ValuePass> pargs;
		// std::map<std::string, ilang::ValuePass> kwargs;

		std::map<ilang::Identifier, ilang::ValuePass> m_args;
		unsigned long m_next_identifier = 0;

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

		void injector(std::map<Identifier, ValuePass>::iterator it) {}
		template<typename T, typename... types> void injector(std::map<Identifier, ValuePass>::iterator it, T &t, types & ... values) {
			assert(it != end());
			it->second->inject(t);
			it++;
			injector(it, values...);
		}

		friend class Function;
	public:
		void set(Context &ctx, Identifier, ilang::ValuePass) override;
		bool has(Context &ctx, Identifier) override;
		ValuePass get(Context &ctx, Identifier i) override;

		Handle<Variable> getVariable(Context &ctx, Identifier i);

		void push(ilang::ValuePass);

		// hm...constructing the context here
		ValuePass operator[](Identifier i) { Context ctx; return get(ctx, i); }
		ValuePass operator[](unsigned long i) { Context ctx; return get(ctx, Identifier(i)); }

		// ValuePass get(std::string);
		// ValuePass get(int);
		// ValuePass operator[](std::string s) { return get(s); }
		// ValuePass operator[](int i) { return get(i); }

		// std::vector<ilang::ValuePass>::iterator begin() { return pargs.begin(); }
		// std::vector<ilang::ValuePass>::iterator end() { return pargs.end(); }

		auto begin() { return m_args.begin(); }
		auto end() { return m_args.end(); }

		size_t size();
		//template<typename... types> void inject(types... &values) {}
		// long a; std::string b; args.inject(a, b);

		Arguments();
		Arguments(const Arguments&);
		Arguments(std::vector<ValuePass> pargs);
		template<typename... types> Arguments(types... values) {
			unwrap(values...);
		}

		template <typename... types> void inject(types & ... values) {
			injector(begin(), values...);
		}
		// int a; std::stinrg b; args.inject(a, b);

	};


	class Function final {
	private:
		bool native = false;
		Function_ptr ptr;
		//		ValuePass object_scope = ValuePass();
		ilang::parserNode::Function *func = NULL;
		friend class Arguments;
		//Context ctx;
		std::map<ilang::Identifier, Handle<Variable> > m_bound;
		void bind_self(Hashable*, bool rebind = false);
		//Scope m_bound;

		// TODO: this should prob just be a pointer to a function rather than
		// a ValuePass
		ValuePass m_alternate; // if the type check fails for this function call alternate

	public:
		void vvv();
		template <typename... types> ilang::ValuePass operator() (Context &ctx, types... values) {
			Arguments args(values...);
			return call(ctx, args);
		}
		ilang::ValuePass operator() (Context &ctx, ilang::Arguments &args) {
			return call(ctx, args);
		}
		ilang::ValuePass operator() (Context &ctx) {
			Arguments args;
			return call(ctx, args);
		}
		ilang::ValuePass call(Context&, ilang::Arguments&);

		IdentifierSet UndefinedElements();

		Function bind(ilang::ValuePass); // bind to an object
		Function bind(Context &ctx);
		Function bind(Hashable*);
		Function rebind(Hashable*);

		Function alternate(ilang::ValuePass);

		Function(const Function &func);
		Function(ilang::parserNode::Function *f, Context &ctx); //, Function_ptr _ptr);
		Function(Function_ptr _ptr);
		Function();
		~Function();
	};

} // namespace ilang


#endif // _ilang_function
