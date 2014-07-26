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
