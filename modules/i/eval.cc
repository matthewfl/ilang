#include "ilang.h"
#include "import.h"
#include "parser.h"
#include "error.h"
#include "parserTree.h"

#include <stdio.h>
#include <string.h>

#include <string>

#include <boost/thread/mutex.hpp>

namespace {
	using namespace ilang;

	boost::mutex Eval_mutex;
	ValuePass Eval_FunctionPass;

	ValuePass evalFunction(Arguments &args) {
		// arguments: ([optional args string], code string)
		// return function with code as body
		error(args.size() != 0, "eval requires at least 1 argument");
		ValuePass str;
		ValuePass arg;
		if(args.size() == 1) {
			str = args[0];
			//	args = ValuePass(new ilang::Value(std::string("")));
		}else if(args.size() == 2) {
			str = args[1];
			arg = args[0];
		}else{
			error(0, "eval expects 1 or 2 arguments");
		}

		std::string code = "_eval_worked = import(\"i.eval\")._eval_loader( { \n";
		if(args.size() == 2) {
			code += "|";
			code += arg->cast<string>();
			code += "| \n";
		}
		code += str->cast<string>();
		code += "} );\n";
		// Use fmemopen to create a FILE* to pass to the parser

		FILE *f = fmemopen(const_cast<char*>(code.c_str()), code.size(), "r");
		assert(f);

		Eval_mutex.lock();

		ilang::parserNode::Head *content = ilang::parser(f, NULL, "EVAL_CONTENT");
		fclose(f);

		content->Link(); // this should call load the functions in the file


		ValuePass ret = Eval_FunctionPass;
		Eval_FunctionPass = ValuePass();
		Eval_mutex.unlock();
		return ret;



	}

	ValuePass evalLoader(Arguments &args) {
		error(args.size() == 1, "eval loader not ment to be called directly");
		//error(args[0]->Get().type() == typeid(long), "eval loader not ment to be called directly");

		Eval_FunctionPass = args[0];

		return valueMaker(true);
	}


	ILANG_LIBRARY_NAME("i/eval",
										 ILANG_FUNCTION("eval", evalFunction)
										 ILANG_FUNCTION("_eval_loader", evalLoader);
										 )

}
