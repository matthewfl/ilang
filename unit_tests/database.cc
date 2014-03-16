#include "base.h"


TEST_CASE("Metadata set and save with dummy", "[database]") {
	init();
	ilang::System_Database->setMeta("testing", "123");

	REQUIRE(ilang::System_Database->getMeta("testing") == "123");
	REQUIRE(ilang::System_Database->getMeta("missing") == "");

	reset();
}
