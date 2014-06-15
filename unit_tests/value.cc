//#include "base.h"
#include "catch.hpp"
#include "value.h"

#include <iostream>
using namespace std;

using namespace ilang;

TEST_CASE("basic value", "[value]") {
	int c = 4;
	ValuePass_new a = ValuePass_new(IntType(c)); //valueMaker(c);

	REQUIRE(a.Get()->type2() == typeid(long));
	cout << a->type2().name() << endl;
	int b;
	//	a->cast(b);
	//REQUIRE(b == 4);
	//a.cast<int>(c);
}

TEST_CASE("basic type", "[value]") {
	IntType a(5);
	REQUIRE(a.type2() == typeid(long));
	int b;
	a.cast(b);
	REQUIRE(b == 5);
	long c;
	a.cast(c);
	REQUIRE(c == 5);
}

TEST_CASE("basic vtable", "[value]") {
	Value_new *v = new IntType(4);
	REQUIRE(v->type2() == typeid(long));
}

TEST_CASE("basic value pass", "[value]") {
	ValuePass_new v(IntType(4));
	REQUIRE(v.Get() == v.operator->());
}
