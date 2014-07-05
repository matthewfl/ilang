#include "base.h"
#include "function.h"

using namespace ilang;


// TEST_CASE("Basic function calling", "[function]") {
// 	auto tree = PARSE_TREE(
// 												 ga = 1;
// 												 fun = {
// 													 assert(0);
// 												 };
// 												 );
// 	tree->Link();
// 	Variable *f = tree->GetScope()->lookup("fun");
// 	INFO("F type is " << f->Get()->Get().type().name());
// 	REQUIRE(f->Get()->Get().type() == typeid(ilang::Function));
// 	ilang::Function fun = boost::any_cast<Function>(f->Get()->Get());
// 	//REQUIRE(fun.native == false);
// 	//REQUIRE(!fun.object);
// 	//REQUIRE(fun.object->Get().type() == typeid(ilang::Object*));
// 	//ValuePass ret = ValuePass(new ilang::Value); // what was I thinking
// 	//ScopePass obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(fun.object->Get())));
// 	//ScopePass obj_scope;
// 	fun();
// 	//std::vector<ValuePass> params;
// 	//fun.ptr(obj_scope, params, &ret);
// 	REQUIRE(asserted == 1);
// }


TEST_CASE("basic native function", "[function]") {
	bool called = false;
	auto fptr = [&called] (Context &ctx,  Arguments &args, ValuePass *ret) {
		called = true;
	};

	ilang::Function f(fptr);
	ValuePass ret = f();

	REQUIRE(called);
}


TEST_CASE("function passing arguments", "[function]") {
	init();
	auto tree = PARSE_TREE(
												 fun = {|a|
														 assert(a);
												 };
												 );
	tree->Link();

	ValuePass fun_ = tree->GetScope()->get("fun");
	ilang::Function fun = *fun_->cast<ilang::Function*>();

	fun(valueMaker(1));
	REQUIRE(!asserted);
}


TEST_CASE("function run", "[function]") {
	init();
	auto tree = PARSE_TREE(
												 fun = {
													 a = 1;
													 b = 5;
													 while(a && b -= 1) {
														 a = 0;
													 }
													 assert(a);
												 };
												 );

	tree->Link();

	ValuePass fun_ = tree->GetScope()->get("fun");
	ilang::Function fun = *fun_->cast<ilang::Function*>();

	fun();
	REQUIRE(asserted);
}
