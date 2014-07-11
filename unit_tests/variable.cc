#include "base.h"


TEST_CASE("Variable modifiers", "[variable]") {
	init();
	RUN_CODE(
					 mod = {|a| assert(a); };
					 mod a = 0;
					 main = {};
					 );
	REQUIRE(asserted);
}
