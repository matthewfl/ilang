#ifndef _ilang_import
#define _ilang_import

#include "debug.h"
#include "variable.h"

#include <string>
#include <vector>
#include <list>
#include <map>

#include <boost/filesystem.hpp>

namespace ilang {
  class Scope;
  class FileScope;
  class Object;
  namespace fs = boost::filesystem;
  class ImportScope;
  extern std::vector<fs::path> ImportSearchPaths;
  extern std::map<fs::path, ImportScope*> ImportedFiles;

  void Init (int argc, char **argv);
  
  class ImportScope {
  private:
    boost::filesystem::path file;
    //std::map<std::string, ImportScope*> childern;
    ImportScope *parent;
  public:
    ImportScope(); // for global
    ImportScope(ImportScope*, boost::filesystem::path);
    boost::filesystem::path locateFile(boost::filesystem::path search);
    void Import (boost::filesystem::path p);
    virtual void load(Object*) {};
  };
  extern ImportScope GlobalImportScope;

  class ImportScopeFile : public ImportScope {
    // for ilang files
  private:
    FileScope *Scope;
    std::list<std::pair<std::list<std::string>, fs::path> > imports;
    Object * GetObject(Scope*, std::list<std::string>);
    Object * GetObject(Object*, std::list<std::string>&);
  public:
    ImportScopeFile(fs::path p);
    void push(std::list<std::string> *pre, std::list<std::string> *name);
    void resolve(Scope*);
    void load(Object*);
    //void provide(FileScope*);
  };
  
  class ImportScopeC : public ImportScope {
    // for C++ files
  public:
    void Local(std::string name, ValuePass val);
  };
}

#endif // _ilang_import
