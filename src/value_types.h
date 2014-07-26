#ifndef _ilang_value_types
#define _ilang_value_types

#include <memory>

#include "value.h"
#include "hashable.h"

namespace ilang {

	class Function;
	class FunctionType : public Value_new {
	public:
		FunctionType(const ilang::Function &f);
		FunctionType(const FunctionType &);
		//FunctionType(FunctionType &&);
		~FunctionType();

		virtual void copyTo(void *d);
		virtual const std::type_info &type();
		virtual ValuePass call(ilang::Arguments &args);

	protected:
		virtual ilang::Function* Cast(cast_chooser<ilang::Function*> c) { return (ilang::Function*)m_ptr; }
	};

	class Hashable;
	class Array;
	class Object;
	class Object;
	class Class;
	class Class_instance;
	class HashableType : public Value_new {
	public:
		HashableType(Handle<Hashable>);
		template<typename T> HashableType(Handle<T> t) {
			auto h = dynamic_pointer_cast<Hashable>(t);
			m_ptr = new Handle<Hashable>(h);
		}
		//HashableType(Handle<Object>);
		//HashableType(Handle<Array>);
		HashableType(const HashableType&);
		~HashableType();

		virtual const std::type_info &type();
		virtual void copyTo(void *d);
		virtual Handle<Hashable> Cast(cast_chooser<Hashable*>);

		ValuePass get(Identifier key);
		void set(Identifier key, ValuePass value);
	};

	class Class;
	class ClassType : public Value_new {
	public:
		ClassType(Handle<Class>);
		ClassType(const ClassType&);
		~ClassType();

		virtual const std::type_info &type();
		virtual void copyTo(void *d);
		virtual Handle<Class> Cast(cast_chooser<ilang::Class*>);
	};

	static auto valueMaker = _valueMaker<
		int, IntType,
		long, IntType,
		size_t, IntType,
		double, FloatType,
		float, FloatType,
		bool, BoolType,
		const char*, StringType,
		//char*, StringType,
		std::string, StringType,
		//std::stringstream, StringType
		ilang::Function, FunctionType,
		Handle<ilang::Class>, ClassType,
		Handle<ilang::Object>, HashableType,
		Handle<ilang::Object>, HashableType,
		Handle<ilang::Class>, HashableType,
		Handle<ilang::Class_instance>, HashableType,
		Handle<ilang::Array>, HashableType,
		Handle<ilang::Hashable>, HashableType
		>();
}

#endif // _ilang_value_type
