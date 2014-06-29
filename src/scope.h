#ifndef _ilang_scope
#define _ilang_scope

#include <map>
#include <string>
#include "variable.h"

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>


namespace ilang {
	//using boost::shared_ptr;

	class Scope;
	typedef boost::shared_ptr<ilang::Scope> ScopePass;
	//typedef Scope* ScopePass;

	class FileScope;
	class Modification;
	template <typename ReturnHook> class FunctionScope;
	class Scope : boost::noncopyable {
		friend class Modification;
	protected:
		template <typename ReturnHook> friend class FunctionScope;
		std::map<std::string, ilang::Variable*> vars;
		ScopePass parent;
		virtual ilang::Variable * _lookup (std::string &name);
	public:
		ilang::Variable * lookup (std::string name);
		ilang::Variable * forceNew (std::string name, std::list<std::string> &modifiers);
		ilang::FileScope * fileScope ();
		virtual void ParentReturn(ValuePass *val) { assert(parent); parent->ParentReturn(val); }
		Scope(ScopePass parent);
		virtual ~Scope();

		int Debug();
	};
	class ImportScopeFile;
	namespace parserNode {
		class Head;
	}
	class FileScope : public Scope {
	private:
		friend class ImportScopeFile;
		friend class Modification;
		parserNode::Head *head;
	protected:
		virtual ilang::Variable * _lookup (std::string &name);
	public:
		FileScope(parserNode::Head *h): Scope(ScopePass()), head(h) {}
		inline parserNode::Head *getHead() { return head; }
	};

	template <typename ReturnHook> class FunctionScope : public Scope {
		friend class Modification;
	public:
		FunctionScope(ScopePass s, ScopePass other, ReturnHook h) : Scope(s), hook(h), objs(other) {}
		void ParentReturn(ValuePass *r) { hook(r); }
	private:
		ReturnHook hook;
		ScopePass objs;
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
	class ObjectScope : public Scope {
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
