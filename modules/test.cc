#include "ilang/ilang.h"

#include <iostream>
using namespace std;

ilang::ValuePass ttt (std::vector<ilang::ValuePass> &args) {
	cout << "ttt function called" << endl;
}


/*class what_class : public ilang::C_Class {
public:
	what_class() {
		register("something", something);
		
	}
};
*/
ILANG_LIBRARY(
	ILANG_FUNCTION("ttt", ttt)
//	ILANG_CLASS("what", what_class)	
)
