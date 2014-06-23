#ifndef _ilang_value_types
#define _ilang_value_types

#include "value.h"


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
		virtual ValuePass_new call(ilang::Arguments &args);
	};

	static auto valueMaker = _valueMaker<
		int, IntType,
		long, IntType,
		double, FloatType,
		float, FloatType,
		bool, BoolType,
		const char*, StringType,
		char*, StringType,
		std::string, StringType,
		//std::stringstream, StringType
		ilang::Function, FunctionType
		>();
}

#endif
