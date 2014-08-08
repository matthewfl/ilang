#define private public
#include "base.h"
#include "import.h"
#include "function.h"
#include "object_new.h"

using namespace ilang;

TEST_CASE("basic import C", "[import]") {
	init();
	auto obj = ImportGetByName("i.test");
	REQUIRE(obj);
	auto ttt = obj->get("ttt");
	REQUIRE(ttt->type() == typeid(ilang::Function));
}

TEST_CASE("basic import i", "[import]") {
	init();
	SET_FILE("i/something",
					 sss = { return 1; };
					 );
	auto obj = ImportGetByName("i.something");
	REQUIRE(obj);
	auto sss = obj->get("sss");
	REQUIRE(sss->type() == typeid(ilang::Function));
}

TEST_CASE("import from other", "[import]") {
	init();
	SET_FILE("tests/another",
					 ggg = { return 10; };
					 );
	RUN_CODE(
					 from tests import another

					 main = {
						 assert(another.ggg() == 10);
					 };
					 );
	REQUIRE(!asserted);
}




// TEST_CASE("module mapping test", "[import][module][mapping]") {
// 	init();

// 	ilang::Object *imp = ImportGetByName("i.map");
// 	REQUIRE(imp);
// 	ilang::Variable *create = imp->operator[]("create");
// 	auto fcreate = boost::any_cast<ilang::Function>(create->Get()->Get());
// 	ilang::Object *obj = new ilang::Object;
// 	obj->members["asdf"] = create;
// 	ValuePass dat(new ilang::Value_Old(obj));
// 	ValuePass ret = fcreate(dat);
// 	ilang::Object *cls = boost::any_cast<ilang::Object*>(ret->Get());
// 	REQUIRE(cls);
// 	bool called = false;
// 	Function mapper([&](ScopePass scope, Arguments& args, ValuePass *ret){
// 			REQUIRE(boost::any_cast<std::string>(args[0]->Get()) == "asdf");
// 			called = true;
// 		});
// 	auto fmap = boost::any_cast<ilang::Function>(cls->operator[]("map")->Get()->Get());
// 	fmap(mapper);
// 	REQUIRE(called);
// }
