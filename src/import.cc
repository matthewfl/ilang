#include "import.h"
#include "scope.h"
#include "variable.h"
#include "object.h"
#include "parserTree.h"
#include "parser.h"
#include "function.h"

#include <stdio.h>
//#include <dlfcn.h>
#include <boost/algorithm/string/replace.hpp>

#include <iostream>
using namespace std;

#include "ilang.h"

namespace ilang {
  std::vector<boost::filesystem::path> ImportSearchPaths;
  std::map<fs::path, ImportScope*> ImportedFiles;
  static std::map<std::string, ImportScope*> *_StaticImportedFiles;
  ImportScope GlobalImportScope;
  bool dl_inited=false;

  std::map<std::string, ImportScope*> & StaticImportedFiles () {
    if(!_StaticImportedFiles) _StaticImportedFiles = new std::map<std::string, ImportScope*>;
    return *_StaticImportedFiles;
  }

  void Init (int argc, char **argv) {
    //debug(0, "init import running");
    //namespace fs = boost::filesystem;

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
    ImportedFiles[fs::absolute(file)] = this; // save the imported files into the list of global imports
  }
  ImportScope::ImportScope() : parent(NULL) {}
  boost::filesystem::path ImportScope::locateFile(boost::filesystem::path search) {
    namespace fs = boost::filesystem;
    cout << "search: " << search << endl;
    auto it = ImportedFiles.find(search);
    if(it != ImportedFiles.end()) {
      return search;
    }
    for(auto it : ImportedFiles) {
      cout << "maps: " << it.first << endl;
    }
    for(auto it2 : StaticImportedFiles()) {
      cout << "static map: " << it2.first << endl;
    }
    //cout << "aaa: " << search.generic_string() << endl;
    auto it2 = StaticImportedFiles().find((std::string)search.c_str());
    if(it2 != StaticImportedFiles().end()) {
      return search;
    }
    fs::path check;
    if(!file.empty()) {
      check = fs::absolute(file).parent_path();
      check += "/";
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
      check += "/";
      check += search;
      check.replace_extension(".i");
      cout << check << " " << fs::is_regular_file(check) <<  endl;
      if(fs::is_regular_file(check)) return check;
      check.replace_extension(".io");
      cout << check << " " << fs::is_regular_file(check) <<  endl;
      if(fs::is_regular_file(check)) return check;
    }
    //ImportedFiles.find(search
    return fs::path();
  }

  ImportScopeFile::ImportScopeFile(fs::path p) : ImportScope(&GlobalImportScope, p) {}

  void ImportScopeFile::push(std::list<std::string> *pre, std::list<std::string> *name) {
    string str;
    fs::path p;
    if(pre) {
      for(auto it : *pre) {
	if(!str.empty()) str += "/";
	str += it;
      }
      // first look for the file name
      // TODO: put this back in when the import system is advance enough to be able to handel importing individual items in
      fs::path look;//(str);
      //p = locateFile(look);
      //if(p.empty()) {
      for(auto it : *name) {
	if(!str.empty()) str += "/";
	str +=	it;
      }
      look = str;
      p = locateFile(look);
      //}
      delete pre; // no longer needed
    }else{
      for(auto it : *name) {
	if(!str.empty()) str += "/";
	str += it;
      }
      fs::path look(str);
      p = locateFile(look);
    }
    assert(! p.empty() ); // not able to find the file that is getting imported
    imports.push_back(pair<std::list<std::string>, fs::path>(*name, p));
    delete name;
  }

  /*void ImportScopeFile::provide(FileScope *scope) {
    scope->vars.find("test");
    }*/

  void ImportScopeFile::load(Object *o) {
    //ilang::Variable *v = o->operator[]("test");
    //v->Set(new ilang::Value((long)123));

    for(auto it : m_Scope->vars) {
      cout << it.first << endl;
      ilang::Variable *v = o->operator[](it.first);
      v->Set(it.second->Get());
    }
  }

  void ImportScope::get(Object *obj, fs::path &pa) {
    auto find = ImportedFiles.find(pa);
    if(find != ImportedFiles.end()) {
      find->second->load(obj);
    }else{
      auto find2 = StaticImportedFiles().find((std::string)pa.c_str());
      if(find2 != StaticImportedFiles().end()) {
	find2->second->load(obj);
      }else{
	// TODO: check if it is a .io file or a .i file
	// need to create a new file and load it in
	fs::path p = fs::absolute(pa);
	if(p.extension() == ".io") {
	  assert(0);
	  // TODO: make the system able to dynamically load in libraries
	  // atm disable this feature due to a number of complications with linking
	  /*	    if(dl_inited == false) {
	  // this is to make the system able to use the symbols that are contained inside this program

	  void *self = dlopen(NULL, RTLD_GLOBAL | RTLD_NOW);
	  //cout << "self " << self << " " << dlerror() << endl;
	  dl_inited=true;
	  }
	  void *handle = dlopen(p.c_str(), RTLD_NOW);
	  cout << p << " " << handle << endl;
	  if(!handle) {
	  cout << "error open library " << dlerror() << endl;
	  }
	  typedef void (*load_fun_t)(ImportScopeC*);
	  load_fun_t load_fun = (load_fun_t) dlsym(handle, "ILANG_LOAD");
	  //if(!load_fun)
	  cout << "error getting function " << dlerror() << endl;
	  cout << flush;
	  ImportScopeC *imp = new ImportScopeC(p);
	  //load_fun(imp);
	  */
	}else{
	  ilang::ImportScopeFile *imp = new ImportScopeFile(p);
	  FILE *f = fopen(p.c_str(), "r");
	  ilang::parserNode::Head *head = ilang::parser(f, imp);
	  fclose(f);
	  head->Link();
	  ImportedFiles.insert(pair<fs::path, ImportScope*>(p, imp));
	  imp->load(obj);
	  // imp is save into the map of ImportedFiles and thus we want it to stay around
	}
      }
    }
  }

  void ImportScopeFile::resolve(Scope *scope) {
    assert(dynamic_cast<FileScope*>(scope));
    m_Scope = dynamic_cast<FileScope*>(scope);
    //std::list<std::string> tt = { "what_", "inthe", "world" };
    //load(GetObject(scope, tt));
    for(auto it : imports) {
      Object *obj = GetObject(scope, it.first);
      get(obj, it.second);
    }
  }

  Object * ImportScopeFile::GetObject(Scope *scope, std::list<std::string> path) {
    assert(!path.empty());
    ilang::Variable *var = scope->lookup(path.front());
    Object *obj;
    if(var->isSet()) {
      boost::any &a = var->Get()->Get();
      assert(a.type() == typeid(ilang::Object*));
      obj = boost::any_cast<ilang::Object*>(a);
    }else{
      ValuePass val = ValuePass(new ilang::Value(obj = new ilang::Object));
      var->Set(val);
    }
    return GetObject(obj, path);
  }

  Object * ImportScopeFile::GetObject(Object *o, std::list<std::string> &path) {
    path.pop_front();
    if(path.empty()) return o;
    ilang::Variable *var = o->operator[](path.front());
    Object *obj;
    if(var->isSet()) {
      boost::any &a = var->Get()->Get();
      assert(a.type() == typeid(ilang::Object*));
      obj = boost::any_cast<ilang::Object*>(a);
    }else{
      ValuePass val = ValuePass(new ilang::Value(obj = new ilang::Object));
      var->Set(val);
    }
    return GetObject(obj, path);

  }

  ImportScopeC::ImportScopeC (char *name) { //: m_name(name) {
    //fs::path p(name);
    StaticImportedFiles().insert(pair<std::string, ImportScope*>(name, this));
  }
  ImportScopeC::ImportScopeC(fs::path p) {
    ImportedFiles.insert(pair<fs::path, ImportScope*>(p, this));
  }
  void ImportScopeC::Set(char *name, ValuePass val) {
    string n = name;
    m_members.insert(pair<std::string, ValuePass>(n, val));
  }
  void ImportScopeC::load(Object *obj) {
    for(auto it : m_members) {
      cout << "load: " << it.first << endl;
      ilang::Variable *v = obj->operator[](it.first);
      v->Set(it.second);
    }
  }
}

namespace ilang {
  ValuePass Function_Creater( ValuePass (*fun)(std::vector<ValuePass>&) ) {
    ilang::Function_ptr f = [fun](Scope *scope, std::vector<ValuePass> & args, ValuePass *ret) {
      *ret = (*fun)(args);
    };
    return ValuePass(new ilang::Value(f));
  }
  ValuePass Function_Creater( ValuePass (*fun)(Scope*, std::vector<ValuePass>&) ) {
    ilang::Function_ptr f = [fun](Scope *scope, std::vector<ValuePass> & args, ValuePass *ret) {
      *ret = (*fun)(scope, args);
    };
    return ValuePass(new ilang::Value(f));
  }
}


namespace {
  using namespace ilang;
  ValuePass ilang_import_get(std::vector<ValuePass> &args) {
    cout << "the ilang_import_get was called\n";
    Object *obj = new Object;

    assert(args.size() == 1);
    assert(args[0]->Get().type() == typeid(std::string));
    std::string name = boost::any_cast<std::string>(args[0]->Get());
    boost::replace_all(name, ".", "\/");
    /*auto it = ImportedFiles.find(name);
      if(it == ImportedFiles.end());
    */
    /*auto it = StaticImportedFiles().find(name);
    if(it != StaticImportedFiles().end()) {
      it->second->load(obj);
      return ValuePass(new ilang::Value(obj));
      }*/
    fs::path p = GlobalImportScope.locateFile(name);
    GlobalImportScope.get(obj, p);
    
    return ValuePass(new ilang::Value(obj));
  }

  ValuePass ilang_import_check(std::vector<ValuePass> &args) {

    return ValuePass(new ilang::Value(false));
  }

  ILANG_LIBRARY_NAME("i/Import",
		     ILANG_FUNCTION("get", ilang_import_get);
		     ILANG_FUNCTION("check", ilang_import_check);
		     )

}
