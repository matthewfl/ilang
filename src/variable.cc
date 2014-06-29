#include "variable.h"
#include "debug.h"
#include "error.h"

#include "function.h" // for defining the Function type

// for the deconstructor of the Value
// TODO: take this out when there is a better solution for dealing with the leaking of Object* and Class* values
#include "object.h"

#include <iostream>
#include <sstream>
using namespace std;

// TODO: make everything use a shared pointer or have some sort of memory heap managing thing to deal with variables going out of scope etc

namespace ilang {
	map<string, std::shared_ptr<Variable_modifier> > ilang_Variable_modifier_list;

	bool Variable::Check (boost::any &a) {
		for(auto it=Modifiers.begin(); it!=Modifiers.end(); it++) {
			assert(*it);
			if(!(*it)->Check(this, a)) {
				error(0, "Check on variable \"" << Name << "\" failed for check \"" << (*it)->Name() << "\"");
				return false;
			}
		}
		return true;
	}
	shared_ptr<Variable_modifier> Variable_modifier::new_variable(Variable *self, std::string name) {
		return shared_ptr<Variable_modifier>();
	}

	Variable::Variable(string name, list<string> modifiers) {
		Name = name;
		//val = ValuePass(NULL);
		for(list<string>::iterator it=modifiers.begin(); it!=modifiers.end(); it++) {
			shared_ptr<Variable_modifier> m = ilang_Variable_modifier_list[*it];
			if(!m)
				cerr << "Variable modifier "<<*it<<" not found\n";
			else {
				shared_ptr<Variable_modifier> vv = m->new_variable(this, name);
				Modifiers.push_back(vv ? vv : m);

			}
		}
	}
	void Variable::Set(ValuePass_Old v) {
		// this most likely will have a lot of calling of constructors and stuff for the shared_ptr
		assert(Check(v->Get()));
		val=v;
		debug(4, v << " " << val );
		for(auto it=Modifiers.begin(); it!=Modifiers.end(); it++) {
			(*it)->Set(this, v->Get());
		}
	}
	ValuePass_Old Variable::Get () {
		ValuePass_Old ret = val;
		if(!ret) ret = ValuePass_Old(new ilang::Value_Old);
		for(auto it=Modifiers.begin(); it!=Modifiers.end(); it++) {
			(*it)->Read(this, ret);
		}
		return ret;
		//return val.get();
	}

	bool Variable::isSet() {
		return val != NULL;
	}

	/*Variable::~Variable () {
		cout << "deleting varaible " << Name << endl;
		}*/

	boost::any & Value_Old::Get() {
		return val;
	}

	Value_Old::Value_Old(boost::any v): val(v) {}
	Value_Old::Value_Old(){}
	Value_Old::~Value_Old() {
		// TODO: fix this to not only work with these classes
		//cout << "destroying Value_Old " << val.type().name() << endl;
		if(val.type() == typeid(ilang::Class*))
			delete boost::any_cast<ilang::Class*>(val);
		else if(val.type() == typeid(ilang::Object*))
			delete boost::any_cast<ilang::Object*>(val);
		else if(val.type() == typeid(ilang::Array*))
			delete boost::any_cast<ilang::Array*>(val);
		//else cout << "nothing deleted\n";
	}
	void Value_Old::Print () {
		// catch the type id to make this faster
		//cout << "inside print " << this << endl;
		if(val.empty()) {
			cout << "printing an empty variable\n";
		}else if(typeid(std::string) == val.type()) {
			cout << boost::any_cast<std::string>(val);
		}else if(typeid(long) == val.type()) {
			cout << boost::any_cast<long>(val);
		}else if(typeid(int) == val.type()) {
			// this should not happen as all ints are currently long
			cout << boost::any_cast<int>(val);
		}else if(typeid(double) == val.type()) {
			cout << boost::any_cast<double>(val);
		}else if(typeid(bool) == val.type()) {
			cout << boost::any_cast<bool>(val) ? "true" : "false" ;
		}else if(typeid(ilang::Object*) == val.type()) {
			ilang::Array *arr;
			if(arr = dynamic_cast<ilang::Array*>(boost::any_cast<ilang::Object*>(val))) {
				bool first = true; cout << "[";
				for(ilang::Variable *vv : arr->members) {
					if(!first) cout << ", "; first = false;
					vv->Get()->Print();
				}
				cout << "]";
			}else{
				cout << "--- CAN NOT PRINT AN OBJECT RAW ---\n";
			}
		}else{
			cout << "could not figure out type: "<< val.type().name() << endl;
		}
		cout <<	 flush;
		//cout << "over the print\n";
	}

	std::string Value_Old::str () {
		stringstream ss;
		if(val.empty()) {
			ss << "--STR OF EMPTY VARIABLE--";
		}else if(typeid(std::string) == val.type()) {
			//ss << boost::any_cast<std::string>(val);
			return boost::any_cast<std::string>(val);
		}else if(typeid(long) == val.type()) {
			ss << boost::any_cast<long>(val);
		}else if(typeid(int) == val.type()) {
			// this should not happen as all ints are currently long
			ss << boost::any_cast<int>(val);
		}else if(typeid(double) == val.type()) {
			ss << boost::any_cast<double>(val);
		}else if(typeid(bool) == val.type()) {
			ss << (boost::any_cast<bool>(val) ? "true" : "false") ;
		}else if(typeid(ilang::Object*) == val.type()) {
			ilang::Array *arr;
			if(arr = dynamic_cast<ilang::Array*>(boost::any_cast<ilang::Object*>(val))) {
				bool first = true; ss << "[";
				for(ilang::Variable *vv : arr->members) {
					if(!first) ss << ", "; first = false;
					ss << vv->Get()->str();
				}
				ss << "]";
			}else{
				ss << "--- CAN NOT PRINT AN OBJECT RAW ---\n";
			}
		}else{
			ss << "--STR OF UNKNOWN TYPE: "<< val.type().name() << "--";
		}
		return ss.str();

	}

	void Value_Old::toJSON(stringstream &ss) {
		if(val.empty()) {
			ss << "null";
		}else if(typeid(std::string) == val.type()) {
			//ss << boost::any_cast<std::string>(val);
			ss << '"' <<	boost::any_cast<std::string>(val) << '"' ;
		}else if(typeid(long) == val.type()) {
			ss << boost::any_cast<long>(val);
		}else if(typeid(int) == val.type()) {
			// this should not happen as all ints are currently long
			ss << boost::any_cast<int>(val);
		}else if(typeid(double) == val.type()) {
			ss << boost::any_cast<double>(val);
		}else if(typeid(bool) == val.type()) {
			ss << boost::any_cast<bool>(val) ? "true" : "false" ;
		}else if(typeid(ilang::Object*) == val.type()) {
			ilang::Array *arr;
			if(arr = dynamic_cast<ilang::Array*>(boost::any_cast<ilang::Object*>(val))) {
				bool first = true; ss << "[";
				for(ilang::Variable *vv : arr->members) {
					if(!first) ss << ", "; first = false;
					vv->Get()->toJSON(ss);
				}
				ss << "]";
			}else{
				ss << "--- CAN NOT PRINT AN OBJECT RAW ---\n";
			}
		}else{
			ss << "--STR OF UNKNOWN TYPE: "<< val.type().name() << "--";
		}
	}

	bool Value_Old::isTrue () {
		if(typeid(bool) == val.type()) {
			return boost::any_cast<bool>(val);
		}else if(typeid(std::string) == val.type()) {
			return boost::any_cast<std::string>(val) != "";
		}else if(typeid(long) == val.type()) {
			return boost::any_cast<long>(val) != 0;
		}else if(typeid(int) == val.type()) {
			return boost::any_cast<int>(val) != 0;
		}else if(typeid(double) == val.type()) {
			return boost::any_cast<double>(val) != 0;
		}else{
			return !val.empty(); // I guess having a value, even if we can't figure out what it is makes it true
		}
	}
}


namespace {
	using namespace ilang;
	class Int_var_type : public ilang::Variable_modifier {
	public:
		char* Name() { return "Int"; }
		bool Check (Variable *self, const boost::any &val) {
			return typeid(int) == val.type() || typeid(long) == val.type();
		}
	};
	ILANG_VARIABLE_MODIFIER(Int, Int_var_type)

	class String_var_type : public ilang::Variable_modifier {
	public:
		char* Name() { return "String"; }
		bool Check (Variable *self, const boost::any &val) {
			return typeid(std::string) == val.type();
		}
	};
	ILANG_VARIABLE_MODIFIER(String, String_var_type)

	class Bool_var_type : public ilang::Variable_modifier {
	public:
		char* Name() { return "Bool"; }
		bool Check (Variable *self, const boost::any &val) {
			return typeid(bool) == val.type();
		}
	};
	ILANG_VARIABLE_MODIFIER(Bool, Bool_var_type)

	class Float_var_type : public ilang::Variable_modifier {
	public:
		char* Name() { return "Float"; }
		bool Check (Variable *self, const boost::any &val) {
			// Should really make this able to promote the variable somehow
			return typeid(double) == val.type();
		}
	};
	ILANG_VARIABLE_MODIFIER(Float, Float_var_type)

	class Number_var_type : public ilang::Variable_modifier {
		char *Name() { return "Number"; }
		bool Check (Variable *self, const boost::any &val) {
			return typeid(double) == val.type() || typeid(long) == val.type() || typeid(int) == val.type();
		}
	};
	ILANG_VARIABLE_MODIFIER(Number, Number_var_type);

	template <char max> class Const_var_type : public ilang::Variable_modifier {
	private:
		char count;
	public:
		char* Name() { return "Const"; }
		Const_var_type() : count(0) {}
		std::shared_ptr<Variable_modifier> new_variable(Variable *v, std::string name) {
			// this will leak when the variable goes out of scope
			// this should not leak now that shared pointer is being used to track this
			std::shared_ptr<Variable_modifier> p ( new Const_var_type<max> );
			return p;
		}
		bool Check(Variable *self, const boost::any &val) {
			if(count >= max) return false;
			count++;
			return true;
		}
	};
	ILANG_VARIABLE_MODIFIER(Const, Const_var_type<1>);
	ILANG_VARIABLE_MODIFIER(Const1, Const_var_type<1>);
	ILANG_VARIABLE_MODIFIER(Const2, Const_var_type<2>);
	ILANG_VARIABLE_MODIFIER(Const3, Const_var_type<3>);
	ILANG_VARIABLE_MODIFIER(Const4, Const_var_type<4>);
	ILANG_VARIABLE_MODIFIER(Const5, Const_var_type<5>);
	ILANG_VARIABLE_MODIFIER(Const6, Const_var_type<6>);
	ILANG_VARIABLE_MODIFIER(Const7, Const_var_type<7>);
	ILANG_VARIABLE_MODIFIER(Const8, Const_var_type<8>);
	ILANG_VARIABLE_MODIFIER(Const9, Const_var_type<9>);


	class Function_var_type : public ilang::Variable_modifier {
	public:
		char *Name() { return "Function"; }
		bool Check(Variable *self, const boost::any &val) {
			return val.type() == typeid(ilang::Function);
		}
	};

	ILANG_VARIABLE_MODIFIER(Function, Function_var_type);


}
