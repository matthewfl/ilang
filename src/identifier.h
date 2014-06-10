#ifndef _ilang_identifier
#define _ilang_identifier

namespace ilang {
	class Identifier {
	private:
		unsigned long m_id;
	public:
		Identifier(std::string);
		Identifier(char*);
		Identifier(unsigned long i) : m_id(i) {}
		bool operator==(Identifier i) { return m_id == i.m_id }
		bool operator<(Identifier i) { return m_id < i.m_id }

	};

}

#endif // _ilang_identifier
