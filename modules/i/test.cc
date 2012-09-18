#include "ilang/ilang.h"
#include "ilang/import.h"
#include "ilang/variable.h"

#include <iostream>
using namespace std;

namespace {
ilang::ValuePass ttt (std::vector<ilang::ValuePass> &args) {
	cout << "ttt function called" << endl;
	debug(5, "what");
}

using namespace ilang;

class wwww : public ilang::C_Class {
private:
  ValuePass something;
public:
  ilang::ValuePass eeee(std::vector<ilang::ValuePass> &args) {
    cout << "calling the eeee function";
  }

  ValuePass set(std::vector<ValuePass> &args) {
    assert(args.size() == 1);
    something = args[0];
    return ValuePass(new ilang::Value);
  }
  ValuePass get(std::vector<ValuePass> &args) {
    return something;
  }
  
  //wwww ();
//};
  wwww ()  {
    reg("eeee", &wwww::eeee);
    reg("set", &wwww::set);
    reg("get", &wwww::get);
    cout << "new class wwww created\n";
  }
};

ILANG_LIBRARY_NAME("i/test",
		   ILANG_FUNCTION("ttt", ttt);
		   ILANG_CLASS("wwww", wwww);
)
}
