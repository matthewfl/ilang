#ifndef _ilang_variable
#define _ilang_variable

#include <map>
#include <string>
#include <list>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

#include "value.h"
#include "value_types.h"

#include "variable_new.h"

namespace ilang {
	//using boost::Handle;

	class Modification;
	class Value_Old;
	class Variable_Old;
	typedef Handle<ilang::Value_Old> ValuePass_Old;
	//typedef ilang::Value_Old* ValuePass;
	class Variable_modifier {
	public:
		virtual bool Check(Variable *self, const boost::any&)=0;
		//	virtual Handle<Variable_modifier> new_variable(Variable *self, std::string);
		virtual void Set(Variable_Old *self, const boost::any &a) {}
		virtual void Read(Variable_Old *self, ValuePass_Old &val) {}
		virtual const char* Name() { return "NOT SET"; }
	};

	extern std::map<std::string, Handle<Variable_modifier> > ilang_Variable_modifier_list;


	// class Variable_Old {
	// private:
	// 	friend class ilang::Modification;
	// 	std::list<Handle<Variable_modifier> > Modifiers;
	// 	std::string Name;
	// 	bool Check (boost::any&);
	// 	ValuePass_Old val;
	// 	Variable_Old()=delete;
	// public:
	// 	Variable_Old (std::string name, std::list<std::string> modifiers);
	// 	void Set(ValuePass_Old v);
	// 	ValuePass_Old Get();
	// 	bool isSet();
	// 	//~Variable();
	// };

	// a wrapper class for any numerical value
	/*
		class Number {
		private:
		enum Type {
		null,
		Int,
		Double
		};
		Type type;
		union {
		long INT;
		double DOUBLE;
		};
		public:
		Number(){type=null;}
		void Set();
		};*/

	class Value_Old {
	private:
		boost::any val;
		friend class Variable;
	public:
		Value_Old (boost::any);
		Value_Old ();
		~Value_Old();
		void Print();
		std::string str();
		void toJSON(std::stringstream&);
		bool isTrue();
		boost::any & Get();
	};


}

#define ILANG_VARIABLE_MODIFIER(name, obj)															\
	namespace { struct _ILANG_VAR_MOD_##name {														\
			_ILANG_VAR_MOD_##name () {																				\
				::ilang::ilang_Variable_modifier_list[ #name ] =								\
					::Handle< ::ilang::Variable_modifier > ( new obj ); \
			}} _ILANG_VAR_MOD_##name##_run;																		\
	}


#endif // _ilang_variable
