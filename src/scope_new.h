#ifndef _ilang_scope_new
#define _ilang_scope_new

#include <map>
#include <vector>
#include <boost/utility.hpp>


#include "identifier.h"
#include "hashable.h"
#include "context.h"
#include "variable_new.h"

#include "helpers.h"


namespace ilang {

	class Scope;
	typedef std::shared_ptr<Scope> ScopePass;

	class Scope : public Hashable, boost::noncopyable {
	private:
		std::map<ilang::Identifier, shared_ptr<Variable> > m_vars;
		Hashable *m_parent;
		ValuePass m_returned;
		//ValuePass m_handle;
		Context *m_ctx;
	protected:
		//ilang::Variable * _lookup(ilang::Identifier);
	public:
		//ilang::Variable * lookup(ilang::Identifier);
		shared_ptr<ilang::Variable> forceNew(ilang::Identifier, std::vector<ValuePass> modifiers);

		// Hashable
		ValuePass get(ilang::Identifier);
		void set(ilang::Identifier, ValuePass);
		bool has(ilang::Identifier);

		Scope(Hashable *h) : m_parent(h) {}
		Scope(Context &ctx) : m_parent(ctx.scope), m_ctx(&ctx) { ctx.scope = this; }
		//Scope(ValuePass p) : m_handle(p), m_parent(p->cast<Hashable*>().get()) {}
		~Scope();

		void Debug();

	};



	ValuePass global_scope_lookup(ilang::Identifier i);
	void global_scope_register(ilang::Identifier i, ValuePass v);

}


#endif // _ilang_scope_new
