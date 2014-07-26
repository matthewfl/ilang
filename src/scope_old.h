#ifndef _ilang_scope
#define _ilang_scope

#include <map>
#include <string>
#include "variable.h"

#include "handle.h"
#include <boost/utility.hpp>


namespace ilang {
	//using boost::Handle;

	class Scope_Old;
	typedef boost::Handle<ilang::Scope_Old> ScopePass_Old;
	//typedef Scope* ScopePass;

	class FileScope;
	class Modification;
	template <typename ReturnHook> class FunctionScope;
	class Scope_Old : boost::noncopyable {
		friend class Modification;
	protected:
		template <typename ReturnHook> friend class FunctionScope;
		std::map<std::string, ilang::Variable*> vars;
		ScopePass_Old parent;
		virtual ilang::Variable * _lookup (std::string &name);
	public:
		ilang::Variable * lookup (std::string name);
		ilang::Variable * forceNew (std::string name, std::list<std::string> &modifiers);
		ilang::FileScope * fileScope ();
		virtual void ParentReturn(ValuePass *val) { assert(parent); parent->ParentReturn(val); }
		Scope_Old(ScopePass_Old parent);
		virtual ~Scope_Old();

		int Debug();
	};

	class ImportScopeFile;
	namespace parserNode {
		class Head;
	}
	class FileScope : public Scope_Old {
	private:
		friend class ImportScopeFile;
		friend class Modification;
		parserNode::Head *head;
	protected:
		virtual ilang::Variable * _lookup (std::string &name);
	public:
		FileScope(parserNode::Head *h): Scope_Old(ScopePass_Old()), head(h) {}
		//inline parserNode::Head *getHead() { return head; }
	};

	template <typename ReturnHook> class FunctionScope : public Scope_Old {
		friend class Modification;
	public:
		FunctionScope(ScopePass_Old s, ScopePass_Old other, ReturnHook h) : Scope_Old(s), hook(h), objs(other) {}
		void ParentReturn(ValuePass *r) { hook(r); }
	private:
		ReturnHook hook;
		ScopePass_Old objs;
	protected:
		virtual ilang::Variable * _lookup(std::string &name) { // the joy of template programming
			auto it = vars.find(name);
			if(it != vars.end())
				return it->second;
			if(objs) {
				ilang::Variable *v;
				v = objs->_lookup(name);
				if(v) return v;
			}
			assert(parent);
			return parent->_lookup(name);
		}
	};

	/*	class ClassScope : public Scope {
			public:
			ClassScope(Scope *s) : Scope(s) {}
			};*/

	class Object;
	class ObjectScope : public Scope_Old {
		friend class Modification;
	private:
		ValuePass hold;
		Object *obj;
	protected:
		virtual ilang::Variable * _lookup(std::string &name);
	public:
		ObjectScope(Object*); // TODO: fix this to have the valuePass
	};

}; // namespace ilang

#endif // _ilang_scope
