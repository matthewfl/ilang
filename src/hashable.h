#ifndef _ilang_hashable
#define _ilang_hashable

#include "identifier.h"
#include "value.h"
#include "variable_new.h"
#include "handle.h"

namespace ilang {
	class Hashable : public Handle_base {
	public:
		virtual ValuePass get(ilang::Identifier)=0;
		virtual void set(ilang::Identifier, ValuePass)=0;
		//virtual void check(ilang::Identifier i, ValuePass v) {}
		//virtual void set_no_check(ilang::Identifier i, ValuePass v) { set(i, v); }
	  virtual bool has(ilang::Identifier)=0;
		virtual Handle<Variable> getVariable(ilang::Identifier)=0;
		virtual ~Hashable(){};
	};
}

#endif // _ilang_hashable
