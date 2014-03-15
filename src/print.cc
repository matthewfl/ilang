#include "print.h"

namespace ilang {
	using namespace std;
	void Printer::tab() {
		for(int i=0;i<indent;i++) {
			*output << "\t";
		}
	}
	ostream & Printer::line () {
		*output << "\n";
		tab();
		return *output;
	}
	ostream & Printer::p() {
		return *output;
	}
	Printer::Printer() {
		output = &cout;
		indent = 0;
	};
	Printer::Printer(ostream *s) {
		output = s;
		indent = 0;
	}
	void Printer::down() {
		assert(indent >= 0);
		indent--;
	}
	void Printer::up() {
		indent++;
	}
}
