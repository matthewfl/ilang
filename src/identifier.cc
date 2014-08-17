#include "identifier.h"

#include <map>
#include <string.h>
#include <sstream>

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
	Identifier::Identifier(const char *str) : Identifier(std::string(str)) {}
	Identifier::Identifier(const std::string str) {
		// TODO: for shorter strings just store there values in the identifier
		// if(str.size() <= 7) {
		// 	m_id = (*((unsigned long*)str.c_str())) + 0x0100000000000000;
		// 	return;
		// }
		auto it = get_identifier_map()->find(str);
		if(it == get_identifier_map()->end()) {
			unsigned long next_identifier = ++starting_identifier;
			get_identifier_map()->insert(std::pair<string, unsigned long>(str, next_identifier));
			m_id = next_identifier;
		}else{
			m_id = it->second;
		}
	}
	std::string Identifier::str() const {
		if(m_id < Identifier_max_int) {
			stringstream ss;
			ss << m_id;
			return ss.str();
		}
		// if(m_id < 0x0200000000000000) {
		// 	long i = (m_id - 0x0100000000000000);
		// 	return std::string((char*)&i);
		// }
		// this is slow as it gets
		for(auto it : *get_identifier_map()) {
			if(it.second == m_id)
				return it.first;
		}
		return "-Not found???-";
	}
}
