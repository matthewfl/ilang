#ifndef _ilang_object
#define _ilang_object

#include <list>
#include <map>
#include <vector>


#include "hashable.h"
#include "identifier.h"
#include "value.h"
#include "variable.h"
#include "handle.h"
#include "context.h"


namespace ilang {

	namespace parserNode {
		class Node;
		class Variable;
	}

	class Object_ish : public Iterable /* Hashable */ {
#ifndef ILANG_MAKE_PUBLIC
	protected:
#else
	public:
#endif
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

	class Class : public Object_ish {
#ifndef ILANG_MAKE_PUBLIC
	private:
#else
	public:
#endif
		std::vector<ValuePass> m_parents;
  public:
		Class();
		Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Context &ctx);
		Class(std::list<ilang::parserNode::Node*> *p, Iterable &iter, Context &ctx);

		ValuePass get(Context &ctx, Identifier i);
		ValuePass builtInGet(Context &ctx, Identifier i);
	};

	class Class_instance : public Object_ish {
#ifndef ILANG_MAKE_PUBLIC
	private:
#else
	public:
#endif
		Handle<Class> m_class;
	public:
		Class_instance(Handle<Class> c);

		ValuePass get(Context &ctx, Identifier i) override;
		bool has(Context &ctx, Identifier i) override;
		friend class Class;
	};

	class Object : public Object_ish {
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

#endif // _ilang_object
