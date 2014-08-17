#ifndef _ilang_identifier
#define _ilang_identifier

#include <string>
//#include <set>
#include <unordered_set>
#include <functional>

namespace ilang {

	const unsigned long Identifier_max_int = 0x0100000000000000;

	class Identifier {
	private:
		unsigned long m_id;
	public:
		Identifier(const char*);
		Identifier(const std::string);
		explicit Identifier(unsigned long i) : m_id(i) {}
		Identifier(const Identifier &i) : m_id(i.m_id) {}
		bool operator==(const Identifier i) const { return m_id == i.m_id; }
		bool operator<(const Identifier i) const { return m_id < i.m_id; }
		bool operator>(const Identifier i) const { return m_id > i.m_id; }
		std::string str() const;
		operator std::string() const { return str(); }
		unsigned long raw() const { return m_id; }
		bool isInt() const { return m_id < Identifier_max_int; }
	};

	typedef std::unordered_set<Identifier> IdentifierSet;
	//	typedef std::set<Identifier> IdentifierSet;

}


// this seems bad....#yolo
namespace std {

	template<>
	struct hash<ilang::Identifier> :
		public __hash_base<size_t, ilang::Identifier>
	{
      size_t
      operator()(ilang::Identifier i) const noexcept
      { return static_cast<size_t>(i.raw()); }
    };

}

#endif // _ilang_identifier
