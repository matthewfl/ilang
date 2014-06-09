//#include "base.h"
#include "catch.hpp"
#include "value.h"

using namespace ilang;

TEST_CASE("basic value", "[value]") {
	int c = 4;
	ValuePass_new a(valueMaker(c));

}
