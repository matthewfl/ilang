#ifndef _ilang_new_object
#define _ilang_new_object

#include "hashable.h"
#include "identifier.h"
#include "value.h"
#include "variable_new.h"
#include <map>
#include <vector>


#include <list>
#include "parserTree.h"

namespace ilang {
	class Class_new : public Hashable {
	private:
		std::vector<ValuePass_new> m_parents;
		std::map<ilang::Identifier, ilang::Variable_new> m_members;

		// std::map<ilang::Identifier, ilang::ValuePass_new> m_members;
		// std::map<ilang::Identifier, std::vector<ilang::ValuePass_new> > m_modifiers;
	public:
		Class_new();
		Class_new(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, ScopePass);
	};

	class Object_new : public Hashable {
	private:
		ValuePass_new m_cls_handle;
		std::map<ilang::Identifier, ilang::ValuePass_new> m_objs;
	public:
		Object_new();

	};

}

#endif // _ilang_new_object
