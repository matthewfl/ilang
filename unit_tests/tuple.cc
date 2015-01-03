#include "base.h"
#include "tuple.h"

using namespace ilang;

TEST_CASE("Basic tuple", "[tuple]") {
	Tuple tup(1,2,3);
	int a,b,c;
	tup.inject(a,b,c);
	REQUIRE(a == 1);
	REQUIRE(b == 2);
	REQUIRE(c == 3);
}

TEST_CASE("Tuple kwargs", "[tuple]") {
	Tuple tup(1, 2, ILANG_KWARG(test), 5, ILANG_KWARG(qw), 10);
	int a, b, c, d;
	tup.inject(a, b, ILANG_KWARG(qw), c, ILANG_KWARG(test), d);
	REQUIRE(a == 1);
	REQUIRE(b == 2);
	REQUIRE(c == 10);
	REQUIRE(d == 5);
}

TEST_CASE("tuple parse", "[tuple]") {
	init();
	RUN_CODE(
					 main = {
						 (a,b) = (1,2);
						 assert(a == 1);
						 assert(b == 2);
					 };
					 );
	REQUIRE(!asserted);
}
