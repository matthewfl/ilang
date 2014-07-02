#include "object.h"
#include "debug.h"
#include "error.h"

// I do not like including this one here, but it is for C_Class->operator[]
#include "ilang.h"

#include <iostream>
using namespace std;

namespace ilang {
	Class::Class(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, ScopePass scope) {
		if(!p && !obj && !scope) return;
		assert(p);
		assert(obj);
		assert(scope);

		for(auto it : *p) {
			assert(dynamic_cast<ilang::parserNode::Value*>(it));
			auto a = dynamic_cast<ilang::parserNode::Value*>(it)->GetValue(scope); //->Get();
			// this will probably need to get changed and stuff, idk
			// this is going to return what ever type is returned by the GetValue from class
			assert(a->type() == typeid(Class*));
			parents.push_back(a->cast<Class*>());
		}
		for(auto it : *obj) {

			Identifier name(it.first->GetFirstName());
			if(members.find(name) != members.end()) {
				debug(0, "Member in class overwritten within self");
				assert(0);
			}
			auto var = make_shared<Variable>();
			//ilang::Variable *var = new Variable(toVector(*(it.first->modifiers)));
			if(it.second) {
				assert(dynamic_cast<ilang::parserNode::Value*>(it.second));
				// TODO:
				//var->Set(dynamic_cast<ilang::parserNode::Value*>(it.second)->GetValue(scope));
			}
			members.insert(pair<Identifier, shared_ptr<Variable> >(name, var));
		}
	}
	Object* Class::NewClass(ValuePass self) {
		assert(0);
		//return new Object(this, self);
	}
	Class::~Class () {
		//for(auto it = members.begin(); it!=members.end(); it++) {
		//	delete it->second;
		//}
	}
	ilang::Variable * Class::operator[](std::string name) {
		// ilang::Variable *var;
		// auto search = members.find(name);
		// if(search == members.end()) {
		// 	for(auto it : parents) {
		// 		var = it->operator[](name);
		// 		if(var) return var;
		// 	}
		// }else{
		// 	return (search->second);
		// }
		return NULL;
	}
	ilang::Variable * Class::operator[](ValuePass val) {
		if(val->type() == typeid(std::string))
			return operator[](val->cast<std::string>());
		assert(0);
		return NULL; // this most likely will get changed in the future
	}

	Object::Object(Class *cla, ValuePass base): baseClassValue(base), baseClass(cla), C_baseClass(NULL), DB_name(NULL) {
		// pass the value as a ValuePass so that it keeps the pointer count that something is using the class incase it is use in an un-named fassion
		// I am not sure what this should look like, if it should check the base classes when accessing stuff or just copy it into a new object when one is created, if a new object is just copied then it would make it hard for the code modification to work

		//assert(base->Get().type() == typeid(Class*));
		//baseClass = boost::any_cast<Class*>(base->Get());

		//std::list<std::string> this_mod = {"Const"};
		//ilang::Variable *this_var = new Variable("this", this_mod);

		// TODO:
		//this_var->Set(ValuePass(new ilang::Value_Old(this)));
		//members.insert(pair<std::string, ilang::Variable*>("this", this_var));
	}
	Object::Object(C_Class *base): C_baseClass(base), baseClass(NULL), DB_name(NULL) {
		// 'this' is not being set in this case, might only be a problem if something out the classes needs something like c.this == c
	}
	Object::Object(): baseClass(NULL), C_baseClass(NULL), DB_name(NULL) {
		std::list<std::string> this_mod = {"Const"};
		//ilang::Variable *this_var = new Variable("this", this_mod);
		// TODO:
		//this_var->Set(ValuePass(new ilang::Value_Old(this)));
		//members.insert(pair<std::string, ilang::Variable*>("this", this_var));
	}
	Object::Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, ScopePass scope): baseClass(NULL), C_baseClass(NULL), DB_name(NULL) {
		assert(obj);
		assert(scope);

		std::list<std::string> this_mod = {"Const"};
		//ilang::Variable *this_var = new Variable("this", this_mod);
		// TODO:
		//this_var->Set(ValuePass(new ilang::Value_Old(this)));
		//members.insert(pair<std::string, ilang::Variable*>("this", this_var));

		for(auto it : *obj) {
			//assert(it.first->name);
			std::string name = it.first->GetFirstName();
			if(members.find(name) != members.end()) {
				debug(0, "Member in Object over written");
				assert(0);
			}
			//ilang::Variable *var = new ilang::Variable(name, *(it.first->modifiers));
			if(it.second) { // != NULL
				assert(dynamic_cast<ilang::parserNode::Value*>(it.second));
				// TODO:
				//var->Set(dynamic_cast<ilang::parserNode::Value*>(it.second)->GetValue(scope));
			}
			//members.insert(pair<std::string, ilang::Variable*>(name, var));
		}
	}

	Object::~Object () {
		//auto tt = members.find("this");
		//if(tt != members.end())
		//	tt->second->Get()->Get() = NULL; // clear out the 'this' variable to pervent the system from crashing when cleaning itself up

		//cout << "deleting obj\n";
		for(auto it=members.begin(); it!=members.end(); it++) {
			//cout << "\t" << it->first << endl;
			delete it->second;
		}

		if(C_baseClass) {
			//	cout << "deleting base class\n";
			delete C_baseClass;
		}
		delete[] DB_name;
	}

	ilang::Variable * Object::operator [] (std::string name) {
		debug_num(5, Debug() );
		auto iter = members.find(name);
		if(iter == members.end()) {
			if(DB_name) {
				//storedData *dat;
				//if(dat = System_Database->Get(DB_name + name)) {
				string use_name = DB_name;
				//cout << "database name: " << use_name << endl;
				use_name += name;
				ilang::Variable *ret;
				//members.insert(pair<std::string, ilang::Variable*>(name, ret = new Variable(use_name, list<string>({"Db"}))));
				//ret->Set(ValuePass(new ilang::Value_Old)); // prime it to make it get the Value_old out of the database
				// for the moment a hack
				return ret;
				//}
				// if there is a database, then there can be the base class thing, we also then need to set the system up to insert the variable into the database
			}
			if(baseClass) {
				// need to make something if the variable is getting set so that it is save differently
				ilang::Variable * var = baseClass->operator[](name);
				if(var) {
					// inserting copy of variable into this object
					// this way any modification will be keep local
					var = new ilang::Variable(*var);
					members.insert(pair<std::string, ilang::Variable*>(name, var));
					return var;
				}
			}else if(C_baseClass) {
				ilang::Variable *var = C_baseClass->operator[](name);
				if(var)
					return var;
			}
			// if we don't find the variable then make a new one and return it
			//debug(0, "Member "<< name << " not found in object");
			// I guess create a new one and insert it
			Variable *var;
			//members.insert(pair<std::string, ilang::Variable*>(name, var = new Variable(name, list<string>())));
			return var;
			//assert(0);

		}

		return (iter->second);
	}

	ilang::Variable * Object::operator[] (ValuePass name) {
		// the interface should be changed so that any value can be looked up in an object
		assert(name->type() == typeid(std::string));
		return operator[](name->cast<std::string>());
	}

	void Object::Debug() {
		for(auto it : members) {
			auto s = it.second->Get();
			//cout << "\t\t" << it.first << "\t" << s->Get().type().name() << "\t" << endl;
			s->Print();
		}
	}

	void Array::Init () {
		Array *self = this;
		std::list<std::string> internal_mods;
		assert(!mem_length); // if the value was set before
		// mem_length = new Variable("length", internal_mods);
		// mem_push = new Variable("push", internal_mods);
		// mem_pop = new Variable("pop", internal_mods);
		// mem_insert = new Variable("insert", internal_mods);
		// mem_remove = new Variable("remove", internal_mods);
		ilang::Function push_fun([self](ScopePass scope, Arguments &args, ValuePass *ret) {
			error(args.size() == 1, "Array.push expects 1 argument");
			assert(self->modifiers);
			//ilang::Variable *var = new ilang::Variable("", *self->modifiers);
			// TODO:
			//var->Set(args[0]);
			//self->members.push_back(var);
			self->RefreshDB();
			*ret = valueMaker(self->members.size());
			});
		// TODO:
		//mem_push->Set(ValuePass(new ilang::Value_Old(push_fun)));

		ilang::Function pop_fun([self](ScopePass scope, Arguments &args, ValuePass *ret) {
			error(args.size() == 0, "Array.pop does not take any arguments");
			ilang::Variable *var = self->members.back();
			// TODO:
			//*ret = var->Get();
			delete var;
			self->members.pop_back();
			self->RefreshDB();
			});
		// TODO:
		//mem_pop->Set(ValuePass(new ilang::Value_Old(pop_fun)));

		ilang::Function insert_fun([self](ScopePass scope, Arguments &args, ValuePass *ret) {
			error(args.size() == 2, "Array.insert expects 2 arguments");
			error(args[0]->type() == typeid(long), "Array.insert expects first argument to be a integer");
			long n = args[0]->cast<long>();
			assert(0); // TODO:
			//ilang::Variable *var = new ilang::Variable("", *self->modifiers);
			//	var->Set(args[1]);
			//auto it = self->members.begin();
			//self->members.insert(it + n, 1, var);
			//self->RefreshDB();
			*ret = valueMaker(self->members.size()); //ValuePass(new ilang::Value_Old(self->members.size()));
			});
		// TODO:
		//mem_insert->Set(ValuePass(new ilang::Value_Old(insert_fun)));

		ilang::Function remove_fun([self](ScopePass scope, Arguments &args, ValuePass *ret) {
			error(args.size() == 1, "Array.remove expects 1 argument");
			error(args[0]->type() == typeid(long), "Array.remove expect argument to be integer");
			long n = args[0]->cast<long>();
			auto it = self->members.begin() + n;
			// TODO:
			//*ret = (*it)->Get();
			delete *it;
			self->members.erase(it);
			self->RefreshDB();
			});
		//mem_remove->Set(ValuePass(new ilang::Value_Old(remove_fun)));
		DB_name = NULL;
	}

	Array::Array (std::list<ilang::parserNode::Node*> *elements, std::list<std::string> *mod, ScopePass scope) :modifiers(mod), mem_length(NULL) {
		members.reserve(elements->size());
		unsigned int count=0;
		for(auto it=elements->begin(); it!= elements->end(); it++) {
			ilang::parserNode::Value *v = dynamic_cast<parserNode::Value*>(*it);
			assert(v);
			string name = "-ARRAY-";
			name += count++;
			//ilang::Variable *var = new Variable(name, *modifiers);
			// TODO:
			//var->Set(v->GetValue(scope));
			//members.push_back(var);
		}
		Init();
	}

	Array::Array(std::vector<ValuePass> &val): mem_length(NULL) {
		members.reserve(val.size());
		modifiers = new std::list<std::string>;
		std::list<std::string> mod;
		for(auto it : val) {
			//ilang::Variable *var = new Variable("", mod);
			// TODO:
			//var->Set(it);
			//members.push_back(var);
		}
		Init();
	}

	Array::Array() {
		modifiers = NULL;
		Init();
	}

	Array::~Array () {
		for(auto it=members.begin();it != members.end(); it++) {
			delete *it;
		}
		// modifiers is owned by another

		// according to val grind, this is still leaking somehow
		delete mem_length;
		delete mem_push;
		delete mem_pop;
		delete mem_remove;
		delete mem_insert;
		delete DB_name;
	}

	ilang::Variable * Array::operator [] (ValuePass name) {
		if(name->type() == typeid(long)) {
			return members[name->cast<long>()];
		}else if(name->type() == typeid(std::string)) {
			return operator[](name->cast<std::string>());
		}
		assert(0);
	}

	// this most likely will leak memory big time until there is some real memory management
	ilang::Variable * Array::operator[] (std::string name) {
		if(name == "length") {
			ilang::Value_Old *val = new ilang::Value_Old((long)members.size());
			//list<string> bm;
			//ilang::Variable * v = new ilang::Variable("length", bm);
			// TODO:
			//mem_length->Set(ValuePass(val));
			return mem_length; // this is going to end up leaking, needs to get fixed etc
		}else if(name == "push") {
			return mem_push;
		}else if(name == "pop") {
			return mem_pop;
		}else if(name == "insert") {
			return mem_insert;
		}else if(name == "remove") {
			return mem_remove;
		}
		// idk what else this can be/should be
		// maybe return functions such as push and pop etc
		error(0, "could not find \""<<name<<"\" in type array");

	}



	ScopeObject::ScopeObject (Scope *_scope, bool isolate) : scope(_scope), Isolate(isolate) {}
	ilang::Variable * ScopeObject::operator [] (std::string name) {
		//
		return scope->lookup(name);
	}
	ilang::Variable * ScopeObject::operator[] (ValuePass val) {
		assert(val->type() == typeid(std::string));
		return operator[](val->cast<std::string>());
	}
}


namespace {
	class Class_var_type : public ilang::Variable_modifier {
	public:
		char* Name() { return "Class"; }
		bool Check (ilang::Variable *self, const boost::any &val) {
			return val.type() == typeid(ilang::Class*);
		}
	};
	ILANG_VARIABLE_MODIFIER(Class, Class_var_type)

	class Object_var_type : public ilang::Variable_modifier {
	public:
		char* Name() { return "Object"; }
		bool Check (ilang::Variable *self, const boost::any &val) {
			return val.type() == typeid(ilang::Object*);
		}
	};

	ILANG_VARIABLE_MODIFIER(Object, Object_var_type)

	class Array_var_type : public ilang::Variable_modifier {
	public:
		char* Name() { return "Array"; }
		bool Check(ilang::Variable *self, const boost::any &val) {
			return val.type() == typeid(ilang::Object*)
				&& dynamic_cast<ilang::Array*>(boost::any_cast<ilang::Object*>(val));
		}
	};

	ILANG_VARIABLE_MODIFIER(Array, Array_var_type);
}
