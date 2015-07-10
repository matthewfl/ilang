#ifndef _ilang_identifier
#define _ilang_identifier

#include <string>
#include <unordered_set>
#include <functional>

namespace ilang {

	const unsigned long Identifier_max_int = 0x0100000000000000;

	class Identifier {
#ifndef ILANG_MAKE_PUBLIC
	private:
#else
	public:
#endif
		unsigned long m_id;
		static std::string lookup(unsigned long id);
		static unsigned long lookup(std::string);
	public:
		Identifier() : m_id(0) {}
		Identifier(const char*);
		Identifier(const std::string);
		explicit Identifier(const unsigned long i) : m_id(i) {}
		Identifier(const Identifier &i) : m_id(i.m_id) {}
		bool operator==(const Identifier i) const { return m_id == i.m_id; }
		bool operator<(const Identifier i) const { return m_id < i.m_id; }
		bool operator>(const Identifier i) const { return m_id > i.m_id; }
		std::string str() const;
		operator std::string() const { return str(); }
		unsigned long raw() const { return m_id; }
		bool isInt() const { return m_id < Identifier_max_int; }
	};


	inline Identifier operator "" _id (const char *c) {
		return Identifier(c);
	}

	typedef std::unordered_set<Identifier> IdentifierSet;
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
