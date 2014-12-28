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
						 assert(j == 0);
						 j = 1;
					 };
					 );
	REQUIRE(!asserted);
	RUN_CODE(
					 DB("j") Int j = 5;
					 main = {
						 assert(j == 1);
					 };
					 );
	REQUIRE(!asserted);
	REQUIRE(ilang::System_Database->Get("j") != NULL);
	reset();
}

TEST_CASE("objects in the database", "[database]") {
	init();
	RUN_CODE(
					 DB("j") j = object {
						 a = 1;
						 b = 3;
					 };
					 main = {
						 assert(j.a == 1);
					 };
					 );
	REQUIRE(!asserted);
	reset();
}
