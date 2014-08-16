#include "base.h"
#include "scope_new.h"
#include "function.h"
#include "value_types.h"

using namespace ilang;
using namespace std;

TEST_CASE("Force New", "[scope]") {
	Context ctx;
	Scope scope(ctx);
	ilang::Function f;
	vector<ValuePass> mods{valueMaker(1), valueMaker(f)};
	auto v = scope.forceNew("test", mods);
}

TEST_CASE("Pre register", "[scope]") {
	RUN_CODE(
					 main = { test(); };
					 test = { assert(0); };
					 );
	REQUIRE(asserted == 1);
}

TEST_CASE("Pre register in function", "[scope]") {
	RUN_CODE(
					 main = {
						 gg = {|a|
									 if(a == 0) return 1;
									 return gg(a - 1);
						 };
						 assert(gg(5) == 1);
					 };
					 );
	REQUIRE(asserted == 1);
}
