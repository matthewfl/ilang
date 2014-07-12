#include "base.h"

#include "parserTree.h"

using namespace ilang;

TEST_CASE( "Run empty function", "[parserTree]" ) {
	using namespace ilang::parserNode;
	init();
	std::list<parserNode::Node*> nodes;

	// all of this is going to leak, yay
	nodes.push_back(
									new AssignExpr(
																 new parserNode::FieldAccess(NULL, "main"),
																 new parserNode::Function(NULL, NULL)
																 ));

	auto head = new Head(&nodes, NULL);
	head->Link();

	head->Run();

	reset();
}

TEST_CASE("Basic assert fail", "[parserTree]") {
	auto head = PARSE_TREE(
												 main = {
													 assert(0);
												 };
												 );
	head->Link();
	head->Run();

	REQUIRE(asserted == 1);
	reset();
}

TEST_CASE("Basic assert passed", "[parserTree]") {
	auto head = PARSE_TREE(
												 main = {
													 assert(1);
												 };
												 );
	head->Link();
	head->Run();

	REQUIRE(asserted == 0);
	reset();
}

TEST_CASE("basic function call", "[parserTree]") {
	auto head = PARSE_TREE(
												 test = { assert(0); };
												 main = { test(); };
												 );
	head->Link();
	head->Run();
	REQUIRE(asserted);
	reset();
}
