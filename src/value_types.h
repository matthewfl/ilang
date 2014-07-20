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
	class Object_new;
	class Class_new;
	class Class_instance;
	class HashableType : public Value_new {
	public:
		HashableType(std::shared_ptr<Hashable>);
		template<typename T> HashableType(std::shared_ptr<T> t) {
			auto h = std::dynamic_pointer_cast<Hashable>(t);
			m_ptr = new std::shared_ptr<Hashable>(h);
		}
		//HashableType(std::shared_ptr<Object>);
		//HashableType(std::shared_ptr<Array>);
		HashableType(const HashableType&);
		~HashableType();

		virtual const std::type_info &type();
		virtual void copyTo(void *d);
		virtual std::shared_ptr<Hashable> Cast(cast_chooser<Hashable*>);

		ValuePass get(Identifier key);
		void set(Identifier key, ValuePass value);
	};

	class Class;
	class ClassType : public Value_new {
	public:
		ClassType(std::shared_ptr<Class>);
		ClassType(const ClassType&);
		~ClassType();

		virtual const std::type_info &type();
		virtual void copyTo(void *d);
		virtual std::shared_ptr<Class> Cast(cast_chooser<ilang::Class*>);
	};

	static auto valueMaker = _valueMaker<
		int, IntType,
		long, IntType,
		size_t, IntType,
		double, FloatType,
		float, FloatType,
		bool, BoolType,
		const char*, StringType,
		char*, StringType,
		std::string, StringType,
		//std::stringstream, StringType
		ilang::Function, FunctionType,
		std::shared_ptr<ilang::Class>, ClassType,
		std::shared_ptr<ilang::Object>, HashableType,
		std::shared_ptr<ilang::Object_new>, HashableType,
		std::shared_ptr<ilang::Class_new>, HashableType,
		std::shared_ptr<ilang::Class_instance>, HashableType,
		std::shared_ptr<ilang::Array>, HashableType,
		std::shared_ptr<ilang::Hashable>, HashableType
		>();
}

#endif // _ilang_value_type
