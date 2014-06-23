#include "identifier.h"

#include <map>
#include <string.h>

using namespace ilang;
using namespace std;

namespace {
	static std::map<std::string, unsigned long> *identifier_map = NULL;
	// TODO: use atomic types
	static unsigned long starting_identifier = 0x0200000000000000;
	static std::map<std::string, unsigned long> *get_identifier_map() {
		if(!identifier_map) {
			identifier_map = new map<string, unsigned long>;
		}
		return identifier_map;
	}
}

namespace ilang {
	Identifier::Identifier(const char *str) {
		if(strlen(str) <= 7) {
			m_id = *((unsigned long*)str) >> 8 + 0x0100000000000000;
			return;
		}
		string s(str);
		auto it = get_identifier_map()->find(s);
		if(it == get_identifier_map()->end()) {
			unsigned long next_identifier = ++starting_identifier;
			get_identifier_map()->insert(std::pair<string, unsigned long>(s, next_identifier));
			m_id = next_identifier;
		}else{
			m_id = it->second;
		}
	}
	Identifier::Identifier(const std::string str) {
		if(str.size() <= 7) {
			m_id = *((unsigned long*)str.c_str()) >> 8 + 0x0100000000000000;
			return;
		}
		auto it = get_identifier_map()->find(str);
		if(it == get_identifier_map()->end()) {
			unsigned long next_identifier = ++starting_identifier;
			get_identifier_map()->insert(std::pair<string, unsigned long>(str, next_identifier));
			m_id = next_identifier;
		}else{
			m_id = it->second;
		}
	}
}
