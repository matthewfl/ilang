#include "base.h"


TEST_CASE("Variable modifiers", "[variable]") {
	init();
	RUN_CODE(
					 mod = {|a| assert(a); };
					 mod a = 0;
					 main = {};
					 );
	REQUIRE(asserted);
}

TEST_CASE("Builtin variable modifiers", "[variable]") {
	init();
	REQUIRE_NOTHROW(
	  RUN_CODE(
					 Int a = 5;
					 main = {};
					 ) ;
	);
	REQUIRE_THROWS_AS(
		RUN_CODE(
						 Int a = 5.5;
						 main = {};
						 ) ;
		, BadType) ;
}
