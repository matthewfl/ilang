#include "base.h"

#include "parserTree.h"

using namespace ilang;

TEST_CASE( "Simple execute", "[parserTree]" ) {
	using namespace ilang::parserNode;
	init();
	ilang::ImportScopeFile scope("test");
	std::list<parserNode::Node*> nodes;
	auto main_name = new std::list<std::string>;
	main_name->push_back("main");

	// all of this is going to leak, yay
	nodes.push_back(
									new AssignExpr(
																 new parserNode::Variable(main_name,
																													new std::list<std::string> // modifier list
																							),
																 new Function(NULL, NULL)
																 ));

	auto head = new Head(&nodes, &scope);
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
