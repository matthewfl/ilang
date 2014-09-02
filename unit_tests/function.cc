#include "base.h"
#include "function.h"
#include "value_types.h"

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
	auto fun_ = tree->GetScope()->get(ctx, "fun");
	REQUIRE(fun_->type() == typeid(ilang::Function));
	REQUIRE(typeid(fun_->cast<ilang::Function*>()) == typeid(ilang::Function*));
	auto fun = *fun_->cast<ilang::Function*>();
	fun(ctx);
	REQUIRE(asserted);
}


TEST_CASE("basic native function", "[function]") {
	bool called = false;
	auto fptr = [&called] (Context &ctx,  Arguments &args, ValuePass *ret) {
		called = true;
	};

	ilang::Function f(fptr);
	ValuePass ret = f(ctx);

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

	ValuePass fun_ = tree->GetScope()->get(ctx, "fun");
	ilang::Function fun = *fun_->cast<ilang::Function*>();

	fun(ctx, valueMaker(1));
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

	ValuePass fun_ = tree->GetScope()->get(ctx, "fun");
	ilang::Function fun = *fun_->cast<ilang::Function*>();

	fun(ctx);
	REQUIRE(asserted);
}


TEST_CASE("function undefined elements", "[function]") {
	init();
	auto tree = PARSE_TREE(
												 fun = {
													 asdf();
													 GG qwer = {};
													 qwer();
												 };
												 );
	tree->Link();
	ValuePass fun_ = tree->GetScope()->get(ctx, "fun");
	ilang::Function fun = *fun_->cast<ilang::Function*>();
	auto undef = fun.UndefinedElements();
	REQUIRE(undef.find("asdf") != undef.end());
	REQUIRE(undef.find("GG") != undef.end());
	// TODO: don't get elements that will be forcedNew
	//REQUIRE(undef.find("qwer") == undef.end());
}

TEST_CASE("function combine", "[function]") {
	init();
	RUN_CODE(
					 fun = {|Int _|
							 return 1;
					 } + {|Float _|
								 return 2;
					 };
					 main = {
						 assert(fun(1) == 1);
						 assert(fun(1.5) == 2);
					 };
					 );
	REQUIRE(!asserted);
}

TEST_CASE("internal value", "[function]") {
	init();
	RUN_CODE(
					 main = {
						 c = 1;
						 assert(c);
					 };
					 );
	REQUIRE(!asserted);
}

TEST_CASE("internal function", "[function]") {
	init();
	RUN_CODE(
					 main = {
						 gg = {
							 c = 1;
							 assert(c);
						 };
						 gg();
					 };
					 );
	REQUIRE(!asserted);
}

// this is vary ambigious, not sure if it should get the value of c or not
// in python if it is reading the value then it will work, but writing to it doesn't, which seems really strange.
// I suppose for the time being this can just not work since it seems like a weird edge case anyways
TEST_CASE("internal value bind", "[function]") {
	init();
	RUN_CODE(
					 main = {
						 gg = {
							 assert(c);
						 };
						 c = 1;
						 gg();
					 };
					 );
	REQUIRE(!asserted);
}

TEST_CASE("internal dont over force", "[function]") {
	init();
	RUN_CODE(
					 main = {
						 gg = {
							 assert(c == 1);
							 c = 2;
						 };
						 c = 1;
						 gg();
						 assert(c == 2);
					 };
					 );
	REQUIRE(!asserted);
}

TEST_CASE("arguments as variale", "[function]") {
	init();
	RUN_CODE(
					 gg = {
						 return arguments;
					 };
					 main = {
						 qq = gg(1,2,3);
						 assert(qq.length == 3);
						 assert(qq[1] == 2);
					 };
					 );
	REQUIRE(!asserted);
}
