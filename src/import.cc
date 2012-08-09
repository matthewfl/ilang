#include "import.h"
#include "scope.h"
#include "variable.h"
#include "object.h"
#include "parserTree.h"
#include "parser.h"

#include <stdio.h>

#include <iostream>
using namespace std;

namespace ilang {
  std::vector<boost::filesystem::path> ImportSearchPaths;
  std::map<fs::path, ImportScope*> ImportedFiles;
  ImportScope GlobalImportScope;

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
    //ImportedFiles.find(search
    return fs::path();
  }

  ImportScopeFile::ImportScopeFile(fs::path p) : ImportScope(&GlobalImportScope, p) {}
  
  void ImportScopeFile::push(std::list<std::string> *pre, std::list<std::string> *name) {
    string str;
    fs::path p;
    if(pre) {
      for(auto it : *pre) {
	str += "/" + it;
      }
      // first look for the file name
      fs::path look(str);
      p = locateFile(look);
      if(p.empty()) {
	for(auto it : *name) {
	  str += "/" + it;
	}
	look = str;
	p = locateFile(look);
      }
      delete pre; // no longer needed
    }else{
      for(auto it : *name) {
	str += "/" + it;
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
    ilang::Variable *v = o->operator[]("test");
    v->Set(new ilang::Value((long)123));

    for(auto it : m_Scope->vars) {
      cout << it.first << endl;
      ilang::Variable *v = o->operator[](it.first);
      v->Set(it.second->Get());
    }
  }

  void ImportScopeFile::resolve(Scope *scope) {
    assert(dynamic_cast<FileScope*>(scope));
    m_Scope = dynamic_cast<FileScope*>(scope);
    std::list<std::string> tt = { "what_", "inthe", "world" };
    load(GetObject(scope, tt));
    for(auto it : imports) {
      Object *obj = GetObject(scope, it.first);
      auto find = ImportedFiles.find(it.second);
      if(find != ImportedFiles.end()) {
	find->second->load(obj);
      }else{
	// TODO: check if it is a .io file or a .i file
	// need to create a new file and load it in
	fs::path p = fs::absolute(it.second);
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
  
}
