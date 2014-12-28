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


TEST_CASE("modifiers call", "[variable]") {
	init();
	RUN_CODE(
					 GreaterThan = {|a| return {|b| return b > a; }; };
					 GreaterThan(10) test = 15;
					 main = {};
					 );
}

TEST_CASE("Class modifier", "[variable]") {
	init();
	RUN_CODE(
					 test = class {
					 check = {|a|
								 assert(a == 5);
							   this.b = a + 1;
					 };
					 setting = {|a|
								 assert(a == 5);
					 };
					 getting = {
								 return this.b;
					 };
					 };
					 test qwer = 5;
					 main = {
						 assert(qwer == 6);
					 };
					 );
	REQUIRE(!asserted);
}
