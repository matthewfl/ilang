#include <iostream>
#include "parser.h"
#include <string.h>
#include "debug.h"

#include "import.h"

using namespace std;

void show_help_info(char *name) {
  cout << "Usage: "<< name << " file name\n"
       << "\t-h\t\tthis help document\n"
       << "\t-f [file]\tthe start file\n"
       << "\t-v #\t\tdebug level\n";
}

int Debug_level=0;

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
      }
    }else{
      if(!main_file)
	main_file = argv[i];
    }
  }
  
  ilang::Init(argc, argv);
  boost::filesystem::path a("/another");
  cout << ilang::GlobalImportScope.locateFile(a) << endl;
  

  /*
  cout << "running file: "<<main_file<<endl;
  FILE *f = fopen(main_file, "r");
  ilang::parser(f, NULL);//->Run();
  fclose(f);
  */

  return 0;
}
