#include "ilang.h"
#include "import.h"
#include "variable_new.h"

#include "handle.h"

#include <iostream>
using namespace std;
using namespace ilang;

namespace {
	ilang::ValuePass ttt (Arguments &args) {
		cout << "ttt function called" << endl;
		debug(5, "what");
	}

	using namespace ilang;

	class wwww : public ilang::C_Class {
	private:
		ValuePass something;
	public:
		ilang::ValuePass eeee(Arguments &args) {
			cout << "calling the eeee function";
		}

		ValuePass set(Arguments &args) {
			assert(args.size() == 1);
			something = args[0];
			return ValuePass();
		}
		ValuePass get(Arguments &args) {
			return something;
		}

		//wwww ();
		//};
		wwww ()	 {
			reg("eeee", &wwww::eeee);
			reg("set", &wwww::set);
			reg("get", &wwww::get);
			cout << "new class wwww created\n";
			//assert(0);
			//while(1);
		}
		virtual ~wwww () {
			cout << "test class deleted\n";
			//assert(0);
		}
	};

	ILANG_LIBRARY_NAME("i/test",
										 ILANG_FUNCTION("ttt", ttt);
										 ILANG_CLASS("wwww", wwww);
										 )
}
