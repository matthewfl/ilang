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

	Identifier main("main");
	Identifier main2("main");
	REQUIRE(main == main2);
	REQUIRE(main.str() == "main");
	REQUIRE(l.str() == "qwerqwerqwerqwer");

	Identifier s("7777777");
	REQUIRE(s.str() == "7777777");
}
