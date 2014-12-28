#include "base.h"
#include "object_new.h"
#include "function.h"
#include "value_types.h"

using namespace ilang;


TEST_CASE("Basic interaction with an object", "[object]") {
	auto obj = valueMaker(make_handle<ilang::Object>());
	Identifier t("test");
	auto v = valueMaker(123);
	obj->set(ctx, t, v);
	auto r = obj->get(ctx, t);
	REQUIRE(r == v);
}

TEST_CASE("Parser tree object", "[object]") {
	auto tree = PARSE_TREE(
												 gg = object {
												   a = 1;
												 };
												 );
	tree->Link();
	auto scope = tree->GetScope();
	auto gg = scope->get(ctx, "gg");
	REQUIRE(gg->type() == typeid(Hashable*));
	auto a = gg->get(ctx, Identifier("a"));
	REQUIRE(a);
	REQUIRE(a->type() == typeid(long));
	long av = a->cast<long>();
	REQUIRE(av == 1);
}

TEST_CASE("Basic interaction with class", "[object]") {
	auto cls = valueMaker(make_handle<ilang::Class>());
	REQUIRE(cls->type() == typeid(Hashable*));
	auto new_fun = cls->get(ctx, Identifier("new"));
	REQUIRE(new_fun->type() == typeid(Function));

}

TEST_CASE("Basic class create", "[object]") {
	RUN_CODE(
					 test = class {
						 a = 1;
					 };

					 main = {
						 assert(test.a == 1);
						 gg = test.new();
						 assert(gg.a == 1);
						 assert(test.instance(gg));
						 assert(test.interface(gg));
					 };
					 );
	REQUIRE(!asserted);
}


TEST_CASE("iterate object", "[object]") {
	auto tree = PARSE_TREE(
												 gg = object {
													 a = 1;
												   b = 2;
												 };
												 );
	tree->Link();
	auto scope = tree->GetScope();
	auto gg_ = scope->get(ctx, "gg");
	auto gg = gg_->cast<Hashable*>();
	Handle<Iterable> ggi = dynamic_pointer_cast<Iterable>(gg);
	REQUIRE(ggi.get() != NULL);
	int c = 0;
	for(auto it : *ggi) {
		c++;
	}
	REQUIRE(c == 2);
}


TEST_CASE("iterate class", "[object]") {
	auto tree = PARSE_TREE(
												 gg = class {
												   a = 1;
												 };
												 ff = class (gg) {
												   b = 2;
												 };
												 );
	tree->Link();
	auto scope = tree->GetScope();
	auto ff = scope->get(ctx, "ff")->cast<Hashable*>();
	Handle<Iterable> ffi = dynamic_pointer_cast<Iterable>(ff);
	REQUIRE(ffi.get() != NULL);
	int c = 0;
	for(auto it : *ffi) {
		c++;
		bool r = it.first == "a" || it.first == "b";
		REQUIRE(r);
	}
	REQUIRE(c == 2);
}

TEST_CASE("bind this", "[object]") {
	init();
	RUN_CODE(
					 gg = object {
					   a = 0;
					   b = { assert(this.a); };
					 };
					 main = {
						 gg.b();
					 };
					 );
	REQUIRE(asserted == 1);
}

TEST_CASE("implicit this bind", "[object]") {
	init();
	RUN_CODE(
					 main = {
						 gg = object {
					     a = 0;
						   b = { assert(a); };
						 };
						 gg.b();
					 };
					 );
	REQUIRE(asserted == 1);
}

TEST_CASE("setting value of member", "[object]") {
	init();
	RUN_CODE(
					 gg = class {
						 a = 1;
						 b = {|gg| a = gg; };
						 c = { return a; };
					 };
					 main = {
						 g = gg.new();
						 assert(g.a == 1);
						 g.b(5);
						 assert(gg.a == 1);
						 //assert(g.a == 5);
						 //assert(g.c() == 5);
					 };
					 );
	REQUIRE(!asserted);
}
