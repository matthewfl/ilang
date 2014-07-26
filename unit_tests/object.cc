#include "base.h"
#include "object_new.h"
#include "function.h"
#include "value_types.h"

using namespace ilang;


TEST_CASE("Basic interaction with an object", "[object]") {
	auto obj = valueMaker(make_handle<ilang::Object>());
	Identifier t("test");
	auto v = valueMaker(123);
	obj->set(t, v);
	auto r = obj->get(t);
	REQUIRE(r == v);
}

TEST_CASE("Parser tree object", "[object]") {
	auto tree = PARSE_TREE(
												 gg = object {
												   a: 1
												 };
												 );
	tree->Link();
	auto scope = tree->GetScope();
	auto gg = scope->get("gg");
	REQUIRE(gg->type() == typeid(Hashable*));
	auto a = gg->get(Identifier("a"));
	REQUIRE(a->type() == typeid(long));
	long av = a->cast<long>();
	REQUIRE(av == 1);
}

TEST_CASE("Basic interaction with class", "[object]") {
	auto cls = valueMaker(make_handle<ilang::Class>());
	REQUIRE(cls->type() == typeid(Hashable*));
	auto new_fun = cls->get(Identifier("new"));
	REQUIRE(new_fun->type() == typeid(Function));

}

// TEST_CASE("Basic interaction with an object", "[object]") {
// 	init();
// 	Object *o = new Object();
// 	ilang::Variable *v = o->operator[](std::string("test"));
// 	//v->Set(ValuePass(new ilang::Value_Old(10)));

// 	v = o->operator[](std::string("test"));
// 	REQUIRE(v->Get()->type() == typeid(int));
// 	REQUIRE(boost::any_cast<int>(v->Get()->Get()) == 10);
// 	delete o;
// }

// TEST_CASE("Basic class", "[object][class]") {
// 	init();
// 	auto tree = PARSE_TREE(
// 												 gg = class {
// 													 Int a: 1
// 												 };
// 												 );
// 	tree->Link();
// 	auto scope = tree->GetScope();
// 	Variable *v = scope->lookup("gg");
// 	INFO("V type is " << v->Get()->Get().type().name());
// 	REQUIRE(v->Get()->Get().type() == typeid(ilang::Class*));
// 	ilang::Class *c = boost::any_cast<ilang::Class*>(v->Get()->Get());
// 	REQUIRE(c);
// 	// ilang::Object *o = new ilang::Object(c, v->Get());
// 	// Variable *a = o->operator[] ("a");
// 	// REQUIRE(a->Get()->Get().type() == typeid(long));
// 	// REQUIRE(boost::any_cast<long>(a->Get()->Get()) == 1);
// }

// TEST_CASE("create class", "[object][class]") {
// 	init();
// 	auto tree = PARSE_TREE(
// 												 gg = class {
// 													 Int a: 1,
// 													 Function b: {|Int c|
// 														 a = c;
// 													 }
// 												 };
// 												 );
// 	tree->Link();
// 	auto scope = tree->GetScope();
// 	auto v = scope->lookup("gg");
// 	REQUIRE(v->Get()->Get().type() == typeid(ilang::Class*));
// 	ilang::Class *c = boost::any_cast<ilang::Class*>(v->Get()->Get());
// 	// ilang::Object *o = new ilang::Object(c, v->Get());
// 	// Variable *a = o->operator[] ("a");
// 	// REQUIRE(a->Get()->Get().type() == typeid(long));
// 	// REQUIRE(boost::any_cast<long>(a->Get()->Get()) == 1);

// }
