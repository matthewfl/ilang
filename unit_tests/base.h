#include "catch.hpp"

#include "database.h"
#include "import.h"

using namespace ilang;

static void init_db() {
	ilang::System_Database = new ilang::DatabaseDummy;
}

static void init() {
	init_db();
	Import_Init(0, NULL);
	//ilang::Init(0, NULL);
}

static void reset() {
	if(ilang::System_Database) {
		delete ilang::System_Database;
		ilang::System_Database = nullptr;
	}
	ilang::ImportSearchPaths.clear();
	ilang::ImportedFiles.clear();
}
