#ifndef _ilang_hashable
#define _ilang_hashable

#include "identifier.h"
#include "value.h"


namespace ilang {
	class Hashable {
	public:
		virtual ValuePass get(ilang::Identifier)=0;
		virtual void set(ilang::Identifier, ValuePass)=0;
	  virtual bool has(ilang::Identifier)=0;
		virtual ~Hashable(){};
	};
}

#endif // _ilang_hashable
