#ifndef _ilang_scope_new
#define _ilang_scope_new

#include <map>
#include <vector>
#include <boost/utility.hpp>


#include "identifier.h"
#include "handle.h"
#include "hashable.h"
#include "context.h"
#include "variable_new.h"
#include "handle.h"

#include "helpers.h"


namespace ilang {

	//class Scope;
	//typedef Handle<Scope> ScopePass;

	class Scope : public Iterable /* Hashable */, boost::noncopyable {
	private:
		std::map<ilang::Identifier, Handle<Variable> > m_vars;

		//ValuePass m_returned;
		//ValuePass m_handle;
	protected:
		Hashable *m_parent;
		Context *m_ctx;
		//ilang::Variable * _lookup(ilang::Identifier);
	public:
		//ilang::Variable * lookup(ilang::Identifier);
		Handle<Variable> forceNew(Context &ctx, ilang::Identifier, std::vector<ValuePass> modifiers);

		// Hashable
		ValuePass get(Context &ctx, ilang::Identifier);
		void set(Context &ctx, ilang::Identifier, ValuePass);
		bool has(Context &ctx, ilang::Identifier);
		Handle<Variable> getVariable(Context &ctx, ilang::Identifier);
		void insert(ilang::Identifier, Handle<Variable>);

		//Scope(Hashable *h) : m_parent(h) {}
		Scope(Context &ctx) : m_parent(ctx.scope), m_ctx(&ctx) { ctx.scope = this; }
		//Scope(ValuePass p) : m_handle(p), m_parent(p->cast<Hashable*>().get()) {}
		~Scope();
		Hashable_iterator begin() override;
		Hashable_iterator end() override;

		void Debug();
	};

	class LocalScope : public Scope {
	protected:
		Hashable *m_local_parent;
	public:
		//LocalScope(Context &ctx) : m_parent(ctx.scope), m_local_parent(ctx.local_scope), m_ctx(&ctx) { ctx.local_scope = this; }
		//~LocalScope();
	};



	ValuePass global_scope_lookup(ilang::Identifier i);
	void global_scope_register(ilang::Identifier i, ValuePass v);

}


#endif // _ilang_scope_new
