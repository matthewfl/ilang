#define CATCH_CONFIG_MAIN
#include "catch.hpp"


extern "C" int Debug_level=0;
extern "C" int get_Debug_level() {
	return Debug_level;
}
