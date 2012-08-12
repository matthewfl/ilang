#include "ilang/ilang.h"
#include "ilang/import.h"
#include "ilang/variable.h"

#include <iostream>
using namespace std;

ilang::ValuePass ttt (std::vector<ilang::ValuePass> &args) {
	cout << "ttt function called" << endl;
	debug(5, "what");
}

ILANG_LIBRARY_NAME("i/test",
	ILANG_FUNCTION("ttt", ttt)
)
