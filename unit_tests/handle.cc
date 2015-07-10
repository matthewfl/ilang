#define ILANG_MAKE_PUBLIC
#include "base.h"
#include "handle.h"


class Handle_test : public Handle_base {
public:
	int *d;
	Handle_test (int *c) : d(c) { (*d)++; }
	~Handle_test () { (*d)++; }
};

TEST_CASE("handle reference count", "[handle]") {
	int c = 0;
	int *cc = &c;
	{
		auto gg = make_handle<Handle_test>(cc);
		REQUIRE(c == 1);
		REQUIRE(gg->ptr_count == 1);
	}
	REQUIRE(c == 2);
}
