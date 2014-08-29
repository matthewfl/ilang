#include "base.h"


TEST_CASE("Metadata set and save with dummy", "[database]") {
	init();
	ilang::System_Database->setMeta("testing", "123");

	REQUIRE(ilang::System_Database->getMeta("testing") == "123");
	REQUIRE(ilang::System_Database->getMeta("missing") == "");

	reset();
}

TEST_CASE("Basic creating db variable", "[database]") {
	init();
	RUN_CODE(
					 DB("j") Int j = 0;
					 main = {
					 };
					 );
	REQUIRE(ilang::System_Database->Get("j") != NULL);
	reset();
}

// TEST_CASE("DB, writing and reading", "[database]") {
// 	init();
// 	RUN_CODE(
// 					 DB Int j = 1;
// 					 main = {};
// 					 );
// 	RUN_CODE(
// 					 DB Int j = 0;
// 					 main = {
// 						 assert(j);
// 					 };
// 					 );
// 	REQUIRE(!asserted);
// 	reset();
// }
