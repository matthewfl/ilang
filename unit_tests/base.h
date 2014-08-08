#ifndef _ilang_unit_base
#define _ilang_unit_base
#include "catch.hpp"


// todo or not to do?? that is the question
//#define private public
//#define protected public

#include "database.h"
#include "import.h"
#include "parserTree.h"
#include "parser.h"

using namespace ilang;

static void init_db() {
	ilang::System_Database = new ilang::DatabaseDummy;
}

static void reset();
static void init() {
	reset();
	init_db();
	Import_Init(0, NULL);
	//ilang::Init(0, NULL);
}

static ilang::parserNode::Head *_build_tree(std::string code, char* fileName=NULL) {
	FILE *f = fmemopen(const_cast<char*>(code.c_str()), code.size(), "r");
	ilang::ImportScopeFile *imp = NULL;
	if(fileName) {
		imp = new ilang::ImportScopeFile(fileName);
		// not quite right, but should work ish
		StaticImportedFiles().insert(std::pair<std::string, ImportScope*>(fileName, imp));
	}

	ilang::parserNode::Head *content = ilang::parser(f, imp, fileName ? fileName : "TEST_CONTENT");
	fclose(f);
	return content;
}



#define PARSE_TREE(...)													\
	_build_tree( #__VA_ARGS__ )

#define SET_FILE(name, ...)											\
	do {																					\
		auto c = _build_tree( #__VA_ARGS__ , name); \
		c->Link();																	\
	} while(0);

#define RUN_CODE(X)																\
	do {																						\
		ilang::parserNode::Head *c = _build_tree( #X , __FILE__ "/gg.i"); \
		c->Link();																													\
		c->Run();																														\
	} while(0);

extern "C" int ilang_Assert_fails;
#define asserted (ilang_Assert_fails - 1)


static void reset() {
	if(ilang::System_Database) {
		delete ilang::System_Database;
		ilang::System_Database = nullptr;
	}
	ilang::ImportSearchPaths.clear();
	ilang::ImportedFiles.clear();
	ilang_Assert_fails = 1;
}


#endif
