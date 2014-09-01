#ifndef _ilang_hashable
#define _ilang_hashable

#include <iterator>
#include <utility>
#include <map>

#include "identifier.h"
#include "value.h"
#include "variable_new.h"
#include "handle.h"

namespace ilang {
	class Hashable : public Handle_base {
	public:
		virtual ValuePass get(ilang::Identifier)=0;
		virtual void set(ilang::Identifier, ValuePass)=0;
		//virtual void check(ilang::Identifier i, ValuePass v) {}
		//virtual void set_no_check(ilang::Identifier i, ValuePass v) { set(i, v); }
	  virtual bool has(ilang::Identifier)=0;
		virtual Handle<Variable> getVariable(ilang::Identifier)=0;
		virtual ~Hashable() {}
	};

	class Hashable_iterator : public std::iterator<std::output_iterator_tag, std::pair<ilang::Identifier, ilang::Handle<ilang::Variable>>> {
	private:
		typedef std::map<ilang::Identifier, ilang::Handle<ilang::Variable> > map_type;
		map_type m_items;
		map_type::iterator it;
	public:
		Hashable_iterator(map_type m) : m_items(m), it(m_items.begin()) {}
		Hashable_iterator(map_type::iterator b) : it(b) {}
		Hashable_iterator& operator++() { ++it; return *this;}
		// copying this might be bad...
		Hashable_iterator operator++(int) { Hashable_iterator tmp(*this); operator++(); return tmp; }
		bool operator==(const Hashable_iterator& rhs) { return it == rhs.it; }
		bool operator!=(const Hashable_iterator& rhs) {return it != rhs.it; }
		std::pair<const ilang::Identifier, ilang::Handle<ilang::Variable>>& operator*() { return *it; }
	};

	class Iterable : public Hashable {
	public:
		virtual Hashable_iterator begin()=0;
		virtual Hashable_iterator end()=0;
	};
}

#endif // _ilang_hashable
