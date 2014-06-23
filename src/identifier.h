#ifndef _ilang_identifier
#define _ilang_identifier

#include <string>

namespace ilang {
	class Identifier {
	private:
		unsigned long m_id;
	public:
		Identifier(const char*);
		explicit Identifier(const std::string);
		explicit Identifier(unsigned long i) : m_id(i) {}
		bool operator==(Identifier i) { return m_id == i.m_id; }
		bool operator<(Identifier i) { return m_id < i.m_id; }
		bool operator>(Identifier i) { return m_id > i.m_id; }
		// TODO: cast back to a string
		std::string str() { return "asdf"; }
		operator std::string() { return str(); }

	};

}

#endif // _ilang_identifier
