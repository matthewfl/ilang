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
	int d = a->cast<int>();
	REQUIRE(d == 4);

	a = valueMaker(1.5);
	REQUIRE(a->type() == typeid(double));
	REQUIRE(a->cast<double>() == 1.5);
}

TEST_CASE("basic type", "[value]") {
	IntType a(5);
	REQUIRE(a.type() == typeid(long));
	int b;
	b = a.cast<int>();
	REQUIRE(b == 5);
	long c;
	c = a.cast<long>();
	REQUIRE(c == 5);

	FloatType f(1.2);
	REQUIRE(f.type() == typeid(double));
	float ff = f.cast<float>() - 1.2;
	REQUIRE(ff < .0001);  // yay floating point math
	REQUIRE(ff > -.0001);
	double dd = f.cast<double>();
	REQUIRE(dd == 1.2);
	int i = f.cast<int>();
	REQUIRE(i == 1);
	//REQUIRE(f.cast<double>() == 1.2);
	//REQUIRE(f.cast<int>() == 1);

	REQUIRE(a.cast<bool>());
	REQUIRE(f.cast<bool>());
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

TEST_CASE("basic cast", "[value]") {
	ValuePass_new v = valueMaker(5);

	int a = v->cast<int>();
	REQUIRE(a == 5);
	REQUIRE(v->cast<long>() == 5);
}

TEST_CASE("basic math", "[value]") {
	ValuePass_new v = valueMaker(5);
	ValuePass_new w = valueMaker(6);

	ValuePass_new g = v + w;

	REQUIRE(g->type() == typeid(long));
	int a = g->cast<int>();
	REQUIRE(a == 11);
}

TEST_CASE("math promote", "[value]") {
	auto v = valueMaker(5);
	auto w = valueMaker(6.5);
	auto g = v + w;
	REQUIRE(g->type() == typeid(double));
	double a = g->cast<double>();
	REQUIRE(a == 11.5);
}
