#ifndef _ilang_import
#define _ilang_import

#include "debug.h"
#include "variable.h"

#include <string>
#include <vector>
#include <map>

#include <boost/filesystem.hpp>

namespace ilang {
  extern std::vector<boost::filesystem::path> ImportSearchPaths;

  void Init (int argc, char **argv);
  
  class ImportScope {
  private:
    boost::filesystem::path file;
    std::map<std::string, ImportScope*> childern;
    ImportScope *parent;
  public:
    ImportScope(); // for global
    ImportScope(ImportScope*, boost::filesystem::path);
    boost::filesystem::path locateFile(boost::filesystem::path search);
  };
  extern ImportScope GlobalImportScope;

  class ImportScopeFile : public ImportScope {
    // for ilang files
  };
  
  class ImportScopeC : public ImportScope {
    // for C++ files
  public:
    void Local(std::string name, ValuePass val);
  };
}

#endif // _ilang_import
