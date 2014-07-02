#ifndef _ilang_context
#define _ilang_context

namespace ilang {
	class ValuePass;
	class Hashable;

	struct Context {
		ValuePass *returned = NULL;
		Hashable *scope;
	};

}

#endif // _ilang_context
