#ifndef _ilang_print
#define _ilang_print

#include <iostream>
#include "debug.h"

namespace ilang {
	class Printer {
	private:
		std::ostream * output;
		int indent;
		void tab();
	public:
		std::ostream & line();
		std::ostream & p(); // print
		Printer();
		Printer(std::ostream*);
		void down();
		void up();
	};
}

#endif // _ilang_print
