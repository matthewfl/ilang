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

TEST_CASE("tuple parse simple", "[tuple]") {
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

TEST_CASE("tuple kwargs", "[tuple]") {
	init();
	RUN_CODE(
					 main = {
						 (a=3,b=4) = (b=1,a=2);
						 assert(a == 2);
						 assert(b == 1);
					 };
					 );
	REQUIRE(!asserted);
}

TEST_CASE("default tuple", "[tuple]") {
	init();
	RUN_CODE(
					 main = {
						 (a=3,b=4) = (b=1,);
						 assert(a == 3);
						 assert(b == 1);
					 };
					 );
	REQUIRE(!asserted);
}

TEST_CASE("types on tuples", "[tuple]") {
	init();
	RUN_CODE(
					 main = {
						 Neq = {|a| {|b| assert(a != b); }; };
						 (Neq(1) e, Neq(2) i) = (i=2, e=1);
					 };
					 );
	REQUIRE(asserted == 2);
}
