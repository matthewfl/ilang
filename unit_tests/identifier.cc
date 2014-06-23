#include "base.h"
#include "identifier.h"

using namespace ilang;

TEST_CASE("basic identifier", "[identifier]") {
	Identifier i(1);
	Identifier j(2);
	REQUIRE(i < j);

	Identifier l("qwerqwerqwerqwer");
	Identifier m("asdfasdfasdfasdf");
	REQUIRE(l < m);
	REQUIRE(i < l);
}
