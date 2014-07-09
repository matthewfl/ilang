#include "base.h"
#include "function.h"

using namespace ilang;


TEST_CASE("Basic function calling", "[function]") {
		auto tree = PARSE_TREE(
												 ga = 1;
												 fun = {
													 assert(0);
												 };
												 );
		tree->Link();
		// omg, this seems bad
		auto fun_ = tree->GetScope()->get("fun");
		REQUIRE(fun_->type() == typeid(ilang::Function));
		REQUIRE(typeid(fun_->cast<ilang::Function*>()) == typeid(ilang::Function*));
		auto fun = *fun_->cast<ilang::Function*>();
		fun();
		REQUIRE(asserted);
}


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
