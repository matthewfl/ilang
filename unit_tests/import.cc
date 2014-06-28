#include "base.h"
#define private public
#include "import.h"
#include "object.h"
#include "function.h"

using namespace ilang;

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
