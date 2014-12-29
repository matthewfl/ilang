#define CATCH_CONFIG_MAIN
#include "catch.hpp"


extern "C" int Debug_level=0;
extern "C" int get_Debug_level() {
	return Debug_level;
}

// by having this value as 1, the assert call won't break when it fails
extern "C" int ilang_Assert_fails = 1;
