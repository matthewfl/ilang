#include "base.h"
#include "object.h"

using namespace ilang;

TEST_CASE("Basic interaction with an object", "[object]") {
	init();
	Object *o = new Object();
	ilang::Variable *v = o->operator[]("test");
	v->Set(ValuePass(new ilang::Value_Old(10)));

	v = o->operator[]("test");
	REQUIRE(v->Get()->Get().type() == typeid(int));
	REQUIRE(boost::any_cast<int>(v->Get()->Get()) == 10);
	delete o;
}

TEST_CASE("Basic class", "[object][class]") {
	init();
	auto tree = PARSE_TREE(
												 gg = class {
													 Int a: 1
												 };
												 );
	tree->Link();
	auto scope = tree->GetScope();
	Variable *v = scope->lookup("gg");
	INFO("V type is " << v->Get()->Get().type().name());
	REQUIRE(v->Get()->Get().type() == typeid(ilang::Class*));
	ilang::Class *c = boost::any_cast<ilang::Class*>(v->Get()->Get());
	REQUIRE(c);
	ilang::Object *o = new ilang::Object(c, v->Get());
	Variable *a = o->operator[] ("a");
	REQUIRE(a->Get()->Get().type() == typeid(long));
	REQUIRE(boost::any_cast<long>(a->Get()->Get()) == 1);
}

TEST_CASE("create class", "[object][class]") {
	init();
	auto tree = PARSE_TREE(
												 gg = class {
													 Int a: 1,
													 Function b: {|Int c|
														 a = c;
													 }
												 };
												 );
	tree->Link();
	auto scope = tree->GetScope();
	auto v = scope->lookup("gg");
	REQUIRE(v->Get()->Get().type() == typeid(ilang::Class*));
	ilang::Class *c = boost::any_cast<ilang::Class*>(v->Get()->Get());
	ilang::Object *o = new ilang::Object(c, v->Get());
	Variable *a = o->operator[] ("a");
	REQUIRE(a->Get()->Get().type() == typeid(long));
	REQUIRE(boost::any_cast<long>(a->Get()->Get()) == 1);

}
