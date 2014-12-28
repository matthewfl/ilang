#ifndef _ilang_new_object
#define _ilang_new_object

#include "hashable.h"
#include "identifier.h"
#include "value.h"
#include "variable_new.h"
#include "handle.h"
#include "context.h"
#include <map>
#include <vector>


#include <list>
//#include "parserTree.h"

namespace ilang {

	namespace parserNode {
		class Node;
		class Variable;
	}

	class Object_ish : public Iterable /* Hashable */ {
	protected:
		std::map<Identifier, Handle<Variable> > m_members;
		Object_ish() {}
	public:
		ValuePass get(Context &ctx, ilang::Identifier) override;
		void set(Context &ctx, ilang::Identifier, ValuePass) override;
		bool has(Context &ctx, ilang::Identifier) override;
		Handle<Variable> getVariable(Context &ctx, ilang::Identifier) override;
		Hashable_iterator begin() override;
		Hashable_iterator end() override;
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
  public:
		Class();
		Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Context &ctx);
		Class(std::list<ilang::parserNode::Node*> *p, Iterable &iter, Context &ctx);

		ValuePass get(Context &ctx, Identifier i);
		ValuePass builtInGet(Context &ctx, Identifier i);
	};

	class Class_instance : public Object_ish {
	private:
		Handle<Class> m_class;
		//	C_Class *m_cclass = NULL;
	public:
		Class_instance(Handle<Class> c);
		//Class_instance(C_Class *c);

		ValuePass get(Context &ctx, Identifier i) override;
		bool has(Context &ctx, Identifier i) override;
		//Handle<Variable> getVariable(Identifier i);
		friend class Class;
	};

	class Object : public Object_ish {
	private:
		//ValuePass m_cls_handle;
		//std::map<ilang::Identifier, ilang::ValuePass> m_objs;
	public:
		Object();
		Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Context &ctx);
		Object(Iterable &iter, Context &ctx);

	};


	class Array : public Hashable {
		std::vector<Handle<Variable> > m_members;
		std::vector<ValuePass> m_modifiers;
	public:
		Array(std::list<ilang::parserNode::Node*> *mods, std::list<ilang::parserNode::Node*> *elems, Context &ctx);
		Array(std::vector<ValuePass> elems);
		ValuePass get(Context &ctx, Identifier) override;
		void set(Context &ctx, Identifier, ValuePass) override;
		bool has(Context &ctx, Identifier) override;
		Handle<Variable> getVariable(Context &ctx, Identifier);
	};

}

#endif // _ilang_new_object
