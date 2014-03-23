#ifndef _ilang_unit_base
#define _ilang_unit_base
#include "catch.hpp"

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

static ilang::parserNode::Head *_build_tree(std::string code) {
	FILE *f = fmemopen(const_cast<char*>(code.c_str()), code.size(), "r");

	ilang::parserNode::Head *content = ilang::parser(f, NULL, "TEST_CONTENT");
	fclose(f);
	return content;
}

#define PARSE_TREE(X)														\
	_build_tree( #X )

#define RUN_CODE(X)																\
	do {																						\
	ilang::parserNode::Head *c = _build_tree( #X ); \
	c->Link();																			\
	c->Run();																				\
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
