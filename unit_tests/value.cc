//#include "base.h"
#include "catch.hpp"
#include "value.h"

#include <iostream>
using namespace std;

using namespace ilang;

TEST_CASE("basic value", "[value]") {
	int c = 4;
	ValuePass_new a = valueMaker(c);

	REQUIRE(a.Get()->type() == typeid(long));
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

TEST_CASE("basic vtable", "[value]") {
	Value_new *v = new IntType(4);
	REQUIRE(v->type() == typeid(long));
	delete v;
}

TEST_CASE("basic value pass", "[value]") {
	ValuePass_new v(IntType(4));
	REQUIRE(v.Get() == v.operator->());
}
