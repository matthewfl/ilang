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

	auto head = new Head(&nodes, NULL);//&scope);
	head->Link();

	head->Run();
	/*
		ilang::Event rootEvent = ilang::global_eventPool()->CreateEvent([&head](void *data) {
	    head.Run();
		});
		rootEvent.Trigger(NULL);
		ilang::global_eventPool()->Run();
	*/

	reset();
}

/*  TODO: re enable
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
*/

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
