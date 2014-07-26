#ifndef _ilang_new_object
#define _ilang_new_object

#include "hashable.h"
#include "identifier.h"
#include "value.h"
#include "variable_new.h"
#include "handle.h"
#include <map>
#include <vector>


#include <list>
#include "parserTree.h"

namespace ilang {

	class Object_ish : public Hashable {
	protected:
		std::map<Identifier, Handle<Variable> > m_members;
		std::weak_ptr<Hashable> m_self;
		Object_ish() {}
	public:
		ValuePass get(ilang::Identifier) override;
		void set(ilang::Identifier, ValuePass) override;
		bool has(ilang::Identifier) override;
		Handle<Variable> getVariable(ilang::Identifier) override;
		virtual ~Object_ish() {}
	};

	// class C_Class : public Object_ish {
	// private:
	// 	//std::map<Identifier, ilang::Variable> m_members;
	// 	std::weak_ptr<C_Class> m_self;
	// protected:
	// 	C_Class() {}
	// 	virtual ~C_Class() {}
	// 	template<typename cls> void reg(Identifier id, ValuePass (cls::*fun)(ilang::Arguments &args) ) {
	// 		assert(m_members.find(id) == m_members.end());
	// 		cls *self = (cls*)this;
	// 		ilang::Function f([fun, self](Context &ctx, ilang::Arguments &args, ValuePass *ret) {
	// 				*ret = (self ->* fun)(args);
	// 				assert(*ret);
	// 			});
	// 		auto var = make_handle<Variable>();
	// 		var->Set(valueMaker(f));
	// 		m_members.insert(std::pair<Identifier, Handle<Variable> >(id, var));
	// 	}

	// };


	class Class : public Object_ish {
	private:
		std::vector<ValuePass> m_parents;
		//std::map<ilang::Identifier, ilang::Variable> m_members;

		// std::map<ilang::Identifier, ilang::ValuePass> m_members;
		// std::map<ilang::Identifier, std::vector<ilang::ValuePass> > m_modifiers;
	public:
		Class();
		//Class_new(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, ScopePass);

		ValuePass get(Identifier i);
	};

	// class Class_instance : public Hashable {
	// private:
	// 	ValuePass m_handle;
	// 	Class_new *m_class = NULL;
	// 	C_Class *m_cclass = NULL;
	// 	std::map<ilang::Identifier, ilang::ValuePass> m_objs;
	// public:
	// 	Class_instance(ValuePass c);
	// 	Class_instance(C_Class *c);

	// };

	class Object : public Object_ish {
	private:
		//ValuePass m_cls_handle;
		//std::map<ilang::Identifier, ilang::ValuePass> m_objs;
	public:
		Object();
		Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Context &ctx);
	};


	class Array : public Hashable {

	};

}

#endif // _ilang_new_object