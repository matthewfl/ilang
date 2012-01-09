#include <iostream>
#include "parser.h"
#include <string.h>
#include "debug.h"

using namespace std;

int Debug_level=5;

int main (int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  if(argc < 2) {
    cout << "Usage: "<<argv[0]<<" file name\n";
    return 1;
  }
  for(int i=1;i<argc;i++) {
    if(*argv[i] == '-') {
      switch(argv[i][1]) {
      case 'v':
	Debug_level = atoi(argv[++i]);
	break;
      }
    }
  }
  cout << "running file: "<<argv[1]<<endl;
  FILE *f = fopen(argv[1], "r");
  ilang::parser(f, NULL);//->Run();
  fclose(f);
  return 0;
}
