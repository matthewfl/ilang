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
		bool operator==(const Identifier i) const { return m_id == i.m_id; }
		bool operator<(const Identifier i) const { return m_id < i.m_id; }
		bool operator>(const Identifier i) const { return m_id > i.m_id; }
		std::string str();
		operator std::string() { return str(); }

	};

}

#endif // _ilang_identifier
