#include "import.h"


namespace ilang {
  namespace {
    class Handle {
    private:
      char *baseDir;
      char *search;
    public:
      Handle(char *b, char *s): baseDir(b), search(s) {}
    };
    static Handle* handle;
  }
  void ImportInit(char *baseDir, char *search) {
    if(!handle)
      handle = new Handle(baseDir, search);
  }
  Import::Import(char *base) {
    
  }
}
