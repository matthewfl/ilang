#ifndef _ilang_variable
#define _ilang_variable

#include "debug.h"
#include "value.h"
#include <vector>

#include "helpers.h"
#include "handle.h"
#include "error.h"

namespace ilang {

	// TODO: make this into a variable instance and have an variable interface
	class Variable : public Handle_base {
	private:
		std::vector<ilang::ValuePass> m_modifiers;
		ilang::ValuePass m_value;
	public:
		Variable(Context &ctx, std::vector<ilang::ValuePass> mod);
		Variable() {}
		Variable(const Variable& v);
		virtual void Set(Context &ctx, ilang::ValuePass);
		virtual ilang::ValuePass Get(Context &ctx);
		virtual void Check(Context &ctx, ilang::ValuePass);
		virtual void SetModifiers(Context &ctx, std::vector<ilang::ValuePass> mod);
	};

	using Variable_ptr = Handle<Variable>;

	inline Variable_ptr make_variable(const ValuePass &val) {
		Context ctx;
		// there are no modifiers, so the context won't be used...
		auto var = make_handle<Variable>();
		var->Set(ctx, val);
		return var;
	}

	class BoundVariable : public Variable {
	private:
		Variable_ptr m_parent;
		ValuePass m_bound;
	public:
		BoundVariable(Variable_ptr p, ValuePass b) : m_parent(p), m_bound(b) {}
		void Set(Context &ctx,ValuePass) override;
		ValuePass Get(Context &ctx) override;
		void Check(Context &ctx, ValuePass) override;
		void SetModifiers(Context &ctx, std::vector<ValuePass>) override;
	};

}

#endif // _ilang_variable
