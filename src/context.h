#ifndef _ilang_context
#define _ilang_context

namespace ilang {
	class ValuePass;
	class Hashable;

	struct Context {
		ValuePass *returned = NULL;
		Hashable *scope = NULL;
		Hashable *local_scope = NULL;


		Hashable *setting_scope = NULL;
		Hashable *getting_scope = NULL;

		Context *parent = NULL;
		Context(Context &p) : parent(&p) {}
		Context() {}
	};
}

#endif // _ilang_context
