#include "ilang/ilang.h"

#include <iostream>
using namespace std;

ilang::Value ttt (std::vector<ilang::Value> args) {
	cout << "ttt function called" << endl;
}


ILANG_LIBRARY(
	ILANG_FUNCTION("ttt", ttt)
)
