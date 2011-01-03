#include <iostream>
#include "parser.h"
#include <string.h>

using namespace std;

int main (int argc, char **argv) {
  if(argc < 2) {
    cout << "Usage: "<<argv[0]<<" file name\n";
    return 1;
  }
  cout << "running file: "<<argv[1]<<endl;
  FILE *f = fopen(argv[1], "r");
  ilang::parser(f, NULL);
  fclose(f);
  return 0;
}
