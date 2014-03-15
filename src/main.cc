#include <iostream>
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include "debug.h"
#include <string>
#include <signal.h>

#include "import.h"
#include "database.h"
#include "parserTree.h"
#include "error.h"
#include "thread.h"

#include "print.h"


namespace ilang { void Init(int, char**); }

//#include "version.h"

using namespace std;

void show_help_info(char *name) {
	cout << "Usage: "<< name << " file name\n"
			 << "\t-h\t\tthis help document\n"
			 << "\t-f [file]\tthe start file\n"
			 << "\t-v #\t\tdebug level\n"
			 << "\t--version\tdispay version\n"
			 << "\t-d\t\tset location of database files\n"
			 << "\t--load\t\t[Variable name]\n"
			 << "\t--dump\t\t[Variable name]\n"
			 << "\n"
			 << "VERSION: " << ILANG_VERSION << "\n";
}

extern "C" int Debug_level=0;
extern "C" int get_Debug_level() {
	return Debug_level;
}

void segfault_singal(int signal, siginfo_t *si, void *arg) {
	ilang::error_print_trace();
	cout << "message from seg fault land\n";
	exit(10);
}

int main (int argc, char **argv) {
	ilang::error_trace main_error("Calling int main, before code has even run");

	// set up system to report error trace on seg fault
	{
		struct sigaction sa;
		memset(&sa, 0, sizeof(struct sigaction));
		sigemptyset(&sa.sa_mask);
		sa.sa_sigaction = segfault_singal;
		sa.sa_flags = SA_SIGINFO;

		sigaction(SIGSEGV, &sa, NULL);
	}

	if(argc < 2) {
		show_help_info(argv[0]);
		return 1;
	}

	char *DB_dump=NULL;
	bool DB_doDump=false;

	char *main_file = NULL;
	boost::filesystem::path db_path = boost::filesystem::current_path();
	db_path += "/DB/";

	for(int i=1;i<argc;i++) {
		if(*argv[i] == '-') {
			switch(argv[i][1]) {
			case 'v':
				Debug_level += atoi(argv[++i]);
				break;
			case 'h':
				show_help_info(argv[0]);
				return 1;
			case 'f':
				main_file = argv[++i];
				break;
			case 'd':
				db_path = argv[++i];
				break;
			case '-':
				string s(argv[i]+2);
				if(s == "version") {
					cerr << "ilang version: ";
					cout << ILANG_VERSION ;
					cerr << endl;
					return 1;
				}else if(s == "load") {
					DB_dump = argv[++i];
					DB_doDump = false;
				}else if(s == "dump") {
					DB_dump = argv[++i];
					DB_doDump = true;
				}else if(s == "help") {
					show_help_info(argv[0]);
					return 1;
				}else{
					cerr << "Argument " << s << " not found\n";
					return 1;
				}
			}
		}else{
			if(!main_file)
				main_file = argv[i];
		}
	}

	if(!main_file && !DB_dump) {
		show_help_info(argv[0]);
		return 1;
	}

	ilang::System_Database = new ilang::DatabaseFile(db_path);

	if(DB_dump) {
		if(DB_doDump) {
			ilang::DatabaseDump(DB_dump, stdout);
		}else{ // load
			ilang::DatabaseLoad(DB_dump, stdin);
		}
		cerr << "DB action complete\n";
		return 0;
	}

	// for setting up the import search path
	ilang::Init(argc, argv);

	//	boost::filesystem::path a("/another");
	//cout << ilang::GlobalImportScope.locateFile(a) << endl;


	ilang::ImportScopeFile *mainImport = new ilang::ImportScopeFile(main_file);

	cout << "running file: "<<main_file<<endl;
	FILE *f = fopen(main_file, "r");
	if(!f) {
		cout << "file \""<<main_file<<"\" not found"<<endl;
		return 1;
	}
	ilang::parserNode::Head *base = ilang::parser(f, mainImport, main_file);
	fclose(f);
	{ ilang::error_trace ee("linking main file");
		base->Link();
	}

	/*	{ ilang::error_trace ee("Printing main parse");
			ilang::Printer pp;
			base->Print(&pp);
			}*/

	/*{ ilang::error_trace ee("running main file");
		base->Run();
		}*/

	//ilang::global_EventPool = new ilang::EventPool;
	//ilang::ThreadPool threads(&events);

	ilang::Event rootEvent = ilang::global_EventPool()->CreateEvent([base](void *data) {
			base->Run();
		});
	rootEvent.Trigger(NULL);

	ilang::global_EventPool()->Run();

	//delete ilang::global_EventPool;
	delete ilang::System_Database;

	return 0;
}
