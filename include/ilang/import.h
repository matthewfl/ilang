#ifndef _ilang_import
#define _ilang_import

namespace ilang {
  class Import {
  public:
    Import(char* base);
    void search(char* name);
  };
}

#endif // _ilang_import
