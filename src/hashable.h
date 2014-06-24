#ifndef _ilang_hashable
#define _ilang_hashable

#include "identifier.h"
#include "value.h"


namespace ilang {
	class Hashable {
	public:
		virtual ValuePass_new get(ilang::Identifier)=0;
		virtual void set(ilang::Identifier, ValuePass_new)=0;
		virtual ~Hashable(){};
	};
}

#endif // _ilang_hashable
