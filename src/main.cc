#include <iostream>
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include "debug.h"
#include <string>

#include "import.h"

#include "parserTree.h"

//#include "version.h"

using namespace std;

void show_help_info(char *name) {
  cout << "Usage: "<< name << " file name\n"
       << "\t-h\t\tthis help document\n"
       << "\t-f [file]\tthe start file\n"
       << "\t-v #\t\tdebug level\n";
}

extern "C" int Debug_level=0;
extern "C" int get_Debug_level() {
  return Debug_level;
}

int main (int argc, char **argv) {
  if(argc < 2) {
    show_help_info(argv[0]);
    return 1;
  }
  char *main_file = NULL;
  for(int i=1;i<argc;i++) {
    if(*argv[i] == '-') {
      switch(argv[i][1]) {
      case 'v':
	Debug_level += atoi(argv[++i]);
	break;
      case 'h':
	show_help_info(argv[0]);
	return 0;
      case 'f':
	main_file = argv[++i];
	break;
      case '-':
	string s(argv[i]+2);
	if(s == "version") {
	  cout << "ilang version: " <<  ILANG_VERSION << endl;
	  return 1;
	}
      }
    }else{
      if(!main_file)
	main_file = argv[i];
    }
  }
  
  ilang::Init(argc, argv);
  //  boost::filesystem::path a("/another");
  //cout << ilang::GlobalImportScope.locateFile(a) << endl;
  
  
  ilang::ImportScopeFile *mainImport = new ilang::ImportScopeFile(main_file);

  cout << "running file: "<<main_file<<endl;
  FILE *f = fopen(main_file, "r");
  if(!f) {
    cout << "file \""<<main_file<<"\" not found"<<endl;
    return 1;
  }
  ilang::parserNode::Head *base = ilang::parser(f, mainImport);
  fclose(f);
  
  base->Link();
  base->Run();
  
  
  return 0;
}
