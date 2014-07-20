#ifndef _ilang_object
#define _ilang_object

#include <string>
#include <vector>
#include <map>
#include "variable.h"
#include "parserTree.h"
#include "scope.h"
#include "hashable.h"


#include "object_new.h"

namespace ilang {
	class Object;
	class Class : public Hashable {
	private:
		std::vector<shared_ptr<Class> > parents;
		std::map<ilang::Identifier, shared_ptr<Variable> > members;
		//std::vector<Class*> parents;
		//std::map<std::string, ilang::Variable*> members;
	public:
		Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, Context&);
		virtual Object* NewClass(ValuePass self);
		// TODO: remove
		ilang::Variable * operator[](std::string name);
		ilang::Variable * operator[](ValuePass);
		virtual ~Class();

		// TODO:
		virtual ValuePass get(ilang::Identifier i) {}
		virtual void set(ilang::Identifier i, ValuePass v) {}
		virtual bool has(ilang::Identifier i) {}// return (bool)get(i); }
		shared_ptr<Variable> getVariable(ilang::Identifier i) override {}
	};
	class ObjectScope;
	//typedef std::string storedData;
	class DB_serializer;
	class C_Class;
	class Object : public Hashable {
	private:
		friend class ObjectScope;
		friend class DB_serializer;
		//friend storedData *DB_createStoredData(const boost::any&);
		//friend ValuePass DB_readStoredData(storedData*);
		ValuePass baseClassValue; // kept so the smart pointer to know that something is using it
		Class *baseClass;
		C_Class *C_baseClass;
		std::map<std::string, ilang::Variable*> members;
		void Debug();
		char *DB_name;
	public:
		Object(Class*, ValuePass);
		Object(C_Class *base);
		Object();
		virtual ~Object();
		Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*>*, Context&);

		// TODO: remove
		virtual ilang::Variable * operator [] (std::string name);
		virtual ilang::Variable * operator [] (ValuePass);

		// TODO:
		ValuePass get(ilang::Identifier i) {}
		void set(ilang::Identifier i, ValuePass v) {}
		bool has(ilang::Identifier i) {}
		shared_ptr<Variable> getVariable(ilang::Identifier i) override {}
	};
	class Array : public Object {
		//friend storedData *DB_createStoredData(const boost::any&);
		//friend ValuePass DB_readStoredData(storedData*);
		friend class DB_serializer;
		friend class Value_Old;
		std::vector<ilang::Variable*> members;
		std::list<std::string> *modifiers;
		ilang::Variable *mem_length;
		ilang::Variable *mem_push;
		ilang::Variable *mem_pop;
		ilang::Variable *mem_insert;
		ilang::Variable *mem_remove;
		void Init();
		void RefreshDB();
		char *DB_name;
	public:
		Array(std::list<ilang::parserNode::Node*>*, std::list<std::string>*, Context&);
		Array(std::vector<ValuePass> &);
		Array();
		ilang::Variable * operator[] (std::string name);
		ilang::Variable * operator[] (ValuePass);
		virtual ~Array();
	};

	class ScopeObject : public Object {
	private:
		Scope *scope;
		bool Isolate;
	public:
		// isolate causes variables that are modified to be put into their own copy of the object rather than the scope
		ScopeObject(Scope*, bool isolate=true);
		ilang::Variable * operator[] (std::string name);
		ilang::Variable * operator[] (ValuePass);
	};
}

#endif // _ilang_object
