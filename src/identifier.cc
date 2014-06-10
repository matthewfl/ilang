#include "identifier.h"

#include <map>

using namespace ilang;
using namespace std;

namespace {
	static std::map<std::string, unsinged long> *identifier_map = NULL;
	static unsigned long starting_identifier = 0x0100000000000000;
	static std::map<std::string, unsigned long> get_identifier_map() {
		if(!identifier_map) {
			identifier_map = new map<string, unsigned long>;
		}
		return identifier_map;
	}
}

namespace ilang {
	Identifier::Identifier(string str) {
		if(str.size() <= 7) {
		}
	}
	Identifier::Identifier(char *
