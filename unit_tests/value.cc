//#include "base.h"
#include "catch.hpp"
#include "value.h"

using namespace ilang;

TEST_CASE("basic value", "[value]") {
	int c = 4;
	auto a = valueMaker(c);


	//a.cast<int>(c);

}
