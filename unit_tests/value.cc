//#include "base.h"
#include "catch.hpp"
#include "value.h"

#include <iostream>
using namespace std;

using namespace ilang;

TEST_CASE("basic value", "[value]") {
	int c = 4;
	ValuePass_new a = valueMaker(c);

	REQUIRE(a->type() == typeid(long));
	cout << a->type().name() << endl;
	int b;
	//	a->cast(b);
	//REQUIRE(b == 4);
	//a.cast<int>(c);
}

TEST_CASE("basic type", "[value]") {
	IntType a(5);
	REQUIRE(a.type() == typeid(long));
	int b;
	a.cast(b);
	REQUIRE(b == 5);
	long c;
	a.cast(c);
	REQUIRE(c == 5);
}
