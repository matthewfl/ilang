#include "ilang.h"
#include "import.h"
#include "database.h"
#include "event.h"



namespace ilang {
	void Init(int argc, char **argv) {

		if(ilang::System_Database == NULL) {
			ilang::System_Database = new ilang::DatabaseFile(boost::filesystem::current_path());
		}
		Import_Init(argc, argv);
	}
}
