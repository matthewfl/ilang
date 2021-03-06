#ifndef _ilang_value_types
#define _ilang_value_types

#include <memory>

#include "value.h"
#include "hashable.h"

namespace ilang {

	class Function;
	class FunctionType : public Value {
	public:
		FunctionType(const ilang::Function &f);
		FunctionType(const FunctionType &);
		~FunctionType();

		virtual void copyTo(void *d) override;
		virtual const std::type_info &type() override;
		virtual ValuePass call(Context &ctx, ilang::Arguments &args) override;

		virtual ValuePass operator + (ValuePass v) override;

	protected:
		virtual ilang::Function* Cast(cast_chooser<ilang::Function*> c) { return (ilang::Function*)m_ptr; }
	};

	class Hashable;
	class Array;
	class Object;
	class Class;
	class Class_instance;
	class Tuple;
	class HashableType : public Value {
	public:
		HashableType(Handle<Hashable>);
		template<typename T> HashableType(Handle<T> t) {
			auto h = dynamic_pointer_cast<Hashable>(t);
			m_ptr = new Handle<Hashable>(h);
		}
		HashableType(const HashableType&);
		~HashableType();

		virtual const std::type_info &type();
		virtual void copyTo(void *d);
		virtual Handle<Hashable> Cast(cast_chooser<Hashable*>);
		virtual Handle<Class> Cast(cast_chooser<Class*>); // TODO: make an new type??
		virtual Handle<Tuple> Cast(cast_chooser<Tuple*>) override;

		ValuePass get(Context &ctx, Identifier key) override;
		void set(Context &ctx, Identifier key, ValuePass value) override;
	};

	// TODO: remove?, class type isn't used
	class Class;
	class ClassType : public Value {
	public:
		ClassType(Handle<Class>);
		ClassType(const ClassType&);
		~ClassType();

		virtual const std::type_info &type();
		virtual void copyTo(void *d);
		virtual Handle<Class> Cast(cast_chooser<ilang::Class*>);
	};

	class VariableType : public Value {
		// special values that control the scopes etc of variables
	public:
		enum types {
			t_normal,
			t_local,
			t_dynamic
		};
		virtual const std::type_info &type() override;
		types getType();
		VariableType(types t);
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
		Identifier, StringType,  // TODO: change
		//std::stringstream, StringType
		ilang::Function, FunctionType,
		//Handle<ilang::Class>, ClassType,
		Handle<ilang::Object>, HashableType,
		//Handle<ilang::Object>, HashableType,
		Handle<ilang::Class>, HashableType,
		Handle<ilang::Class_instance>, HashableType,
		Handle<ilang::Array>, HashableType,
		Handle<ilang::Hashable>, HashableType,
		Handle<ilang::Tuple>, HashableType
		//Handle<ilang::Arguments>, HashableType
		>();
}

#endif // _ilang_value_type
