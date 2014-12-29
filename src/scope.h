#ifndef _ilang_scope
#define _ilang_scope

#include <map>
#include <vector>
#include <boost/utility.hpp>


#include "identifier.h"
#include "handle.h"
#include "hashable.h"
#include "context.h"
#include "variable.h"
#include "handle.h"

#include "helpers.h"


namespace ilang {

	class Scope : public Iterable /* Hashable */, boost::noncopyable {
	private:
		std::map<ilang::Identifier, Handle<Variable> > m_vars;
	protected:
		Hashable *m_parent;
		Context *m_ctx;
	public:
		Handle<Variable> forceNew(Context &ctx, ilang::Identifier, std::vector<ValuePass> modifiers);

		// Hashable
		ValuePass get(Context &ctx, ilang::Identifier);
		void set(Context &ctx, ilang::Identifier, ValuePass);
		bool has(Context &ctx, ilang::Identifier);
		Handle<Variable> getVariable(Context &ctx, ilang::Identifier);
		void insert(ilang::Identifier, Handle<Variable>);

		Scope(Context &ctx) : m_parent(ctx.scope), m_ctx(&ctx) { ctx.scope = this; }
		~Scope();
		Hashable_iterator begin() override;
		Hashable_iterator end() override;

		void Debug();
	};

	ValuePass global_scope_lookup(ilang::Identifier i);
	void global_scope_register(ilang::Identifier i, ValuePass v);

}


#endif // _ilang_scope
