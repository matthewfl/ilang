#include "import.h"

#include <iostream>
using namespace std;

namespace ilang {
  std::vector<boost::filesystem::path> ImportSearchPaths;
  ImportScope GlobalImportScope;

  void Init (int argc, char **argv) {
    //debug(0, "init import running");
    namespace fs = boost::filesystem;
    
    ImportSearchPaths.push_back(fs::current_path());
    
    fs::path mod = fs::current_path();
    mod /= "/modules";
    ImportSearchPaths.push_back(mod);

#ifdef __linux__
    // the path of the exe file
    fs::path exe ( "/proc/self/exe" );
    exe = fs::absolute(fs::read_symlink(exe));
    exe.remove_filename();
    exe /= "/modules";
    ImportSearchPaths.push_back(exe);
#endif
  }

  ImportScope::ImportScope(ImportScope *_parent, boost::filesystem::path _file) : file(_file), parent(_parent) {
    if(!parent) {
      parent = &GlobalImportScope;
    }
  }
  ImportScope::ImportScope() : parent(NULL) {}
  boost::filesystem::path ImportScope::locateFile(boost::filesystem::path search) {
    namespace fs = boost::filesystem;
    fs::path check;
    if(!file.empty()) {
      check = file;
      check += search;
      check.replace_extension(".i");
      cout << check << " " << fs::is_regular_file(check) <<  endl;
      if(fs::is_regular_file(check)) return check;
      check.replace_extension(".io");
      cout << check << " " << fs::is_regular_file(check) <<  endl;
      if(fs::is_regular_file(check)) return check;
      return parent->locateFile(search);
    }
    for(auto it : ImportSearchPaths) {
      check = it;
      check += search;
      check.replace_extension(".i");
      cout << check << " " << fs::is_regular_file(check) <<  endl;
      if(fs::is_regular_file(check)) return check;
      check.replace_extension(".io");
      cout << check << " " << fs::is_regular_file(check) <<  endl;
      if(fs::is_regular_file(check)) return check;
      
    }
    return fs::path();
  }
  
}
