#include <stdio.h>
//#include <dlfcn.h>
#include <boost/algorithm/string/replace.hpp>

#include <iostream>

#include "import.h"
#include "scope.h"
#include "variable.h"
#include "object.h"
#include "parserTree.h"
#include "parser.h"
#include "function.h"
#include "error.h"

#include "ilang.h"

using namespace std;


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

	void Import_Init (int argc, char **argv) {
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
#else
#warning "Path local to the execuitable location will not be included"
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
		//cout << "search: " << search << endl;
		auto it = ImportedFiles.find(search);
		if(it != ImportedFiles.end()) {
			return search;
		}
		/*
			for(auto it : ImportedFiles) {
			cout << "maps: " << it.first << endl;
			}
			for(auto it2 : StaticImportedFiles()) {
			cout << "static map: " << it2.first << endl;
			}
		*/
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
			cout << check << " " << fs::is_regular_file(check) <<	 endl;
			if(fs::is_regular_file(check)) return check;
			check.replace_extension(".io");
			cout << check << " " << fs::is_regular_file(check) <<	 endl;
			if(fs::is_regular_file(check)) return check;
			return parent->locateFile(search);
		}
		for(auto it : ImportSearchPaths) {
			check = it;
			check += "/";
			check += search;
			check.replace_extension(".i");
			cout << check << " " << fs::is_regular_file(check) <<	 endl;
			if(fs::is_regular_file(check)) return check;
			check.replace_extension(".io");
			cout << check << " " << fs::is_regular_file(check) <<	 endl;
			if(fs::is_regular_file(check)) return check;
		}
		return fs::path();
	}

	ImportScopeFile::ImportScopeFile(fs::path p) : ImportScope(&GlobalImportScope, p) {}

	void ImportScopeFile::push(std::vector<Identifier> *pre, std::vector<Identifier> *name) {
		string str;
		fs::path p;
		if(pre) {
			for(auto it : *pre) {
				if(!str.empty()) str += "/";
				str += it.str();
			}
			// first look for the file name
			// TODO: put this back in when the import system is advance enough to be able to handel importing individual items in
			fs::path look;//(str);
			//p = locateFile(look);
			//if(p.empty()) {
			for(auto it : *name) {
				if(!str.empty()) str += "/";
				str +=	it.str();
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
		error(! p.empty(), "not able to locate file " << str << " for importing");
		imports.push_back(pair<std::vector<Identifier>, fs::path>(*name, p));
		delete name;
	}

	void ImportScopeFile::load(Context &ctx, Handle<Hashable> o) {
		for(auto it : *m_head->GetScope()) {
			o->set(ctx, it.first, it.second->Get(ctx));
		}
	}

	void ImportScope::get(Context &ctx, Handle<Hashable> obj, fs::path &pa) {
		auto find = ImportedFiles.find(pa);
		if(find != ImportedFiles.end()) {
			find->second->load(ctx, obj);
		}else{
			auto find2 = StaticImportedFiles().find((std::string)pa.c_str());
			if(find2 != StaticImportedFiles().end()) {
				find2->second->load(ctx, obj);
			}else{
				// TODO: check if it is a .io file or a .i file
				// need to create a new file and load it in
				fs::path p = fs::absolute(pa);
				if(p.extension() == ".io") {
					assert(0);
					// TODO: make the system able to dynamically load in libraries
					// atm disable this feature due to a number of complications with linking
					/*			if(dl_inited == false) {
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
					// TODO: error handling if there is no file
					ilang::ImportScopeFile *imp = new ImportScopeFile(p);
					FILE *f = fopen(p.c_str(), "r");
					ilang::parserNode::Head *head = ilang::parser(f, imp, p.c_str());
					fclose(f);
					head->Link();
					ImportedFiles.insert(pair<fs::path, ImportScope*>(p, imp));
					imp->load(ctx, obj);
					// imp is save into the map of ImportedFiles and thus we want it to stay around
				}
			}
		}
	}

	void ImportScopeFile::resolve(Context &ctx) {
		for(auto it : imports) {
			// this looks wrong?
			auto obj = GetObject(ctx, it.first);
			get(ctx, obj, it.second);
		}
	}

	Handle<Hashable> ImportScopeFile::GetObject(Context &ctx, std::vector<Identifier> path) {
		ValuePass obj;
		Handle<Hashable> o;
		if(ctx.scope->has(ctx, path.front())) {
			obj = ctx.scope->get(ctx, path.front());
			assert(obj->type() == typeid(Hashable*));
			o = obj->cast<Hashable*>();
		}else{
			o = make_handle<Object>();
			obj = valueMaker(o);
			ctx.scope->set(ctx, path.front(), obj);
		}
		return GetObject(ctx, o, path);
	}

	Handle<Hashable> ImportScopeFile::GetObject(Context &ctx, Handle<Hashable> obj, std::vector<Identifier> &path) {
		path.erase(path.begin());
		if(path.empty()) return obj;
		ValuePass val;
		Handle<Hashable> nex;
		if(obj->has(ctx, path.front())) {
			val = obj->get(ctx, path.front());
			nex = val->cast<Hashable*>();
		}else{
			nex = make_handle<Object>();
			val = valueMaker(nex);
			obj->set(ctx, path.front(), val);
		}
		return GetObject(ctx, nex, path);
	}

	ImportScopeC::ImportScopeC (char *name) {
		StaticImportedFiles().insert(pair<std::string, ImportScope*>(name, this));
	}
	ImportScopeC::ImportScopeC(fs::path p) {
		ImportedFiles.insert(pair<fs::path, ImportScope*>(p, this));
	}
	void ImportScopeC::Set(char *name, ValuePass val) {
		string n = name;
		m_members.insert(pair<std::string, ValuePass>(n, val));
	}
	void ImportScopeC::load(Context &ctx, Handle<Hashable> obj) {
		for(auto it : m_members) {
			obj->set(ctx, Identifier(it.first), it.second);
		}
	}



	Handle<ilang::Object> ImportGetByName(std::string name) {
		Context ctx; // TODO: pass the context here?
		boost::replace_all(name, ".", "\/");
		fs::path p = GlobalImportScope.locateFile(name);
		if(p.empty()) return NULL;
		auto obj = make_handle<Object>();
		GlobalImportScope.get(ctx, obj, p);

		return obj;
	}
}

namespace {
	using namespace ilang;
	ValuePass ilang_import_get(Context &ctx, Arguments &args) {
		auto obj = make_handle<Object>();

		error(args.size() == 1, "i.Import.check expects 1 argument");
		error(args[0]->type() == typeid(std::string), "i.Import.check expects a string");

		std::string name = args[0]->cast<std::string>();
		boost::replace_all(name, ".", "\/");
		fs::path p = GlobalImportScope.locateFile(name);
		GlobalImportScope.get(ctx, obj, p);

		return valueMaker(obj);
	}

	ValuePass ilang_import_check(Context &ctx, Arguments &args) {
		error(args.size() == 1, "i.Import.check expects 1 argument");
		error(args[0]->type() == typeid(std::string), "i.Import.check expects a string");
		std::string name = args[0]->cast<std::string>();
		boost::replace_all(name, ".", "\/");

		fs::path p = GlobalImportScope.locateFile(name);

		return valueMaker(! p.empty());
	}

	ILANG_LIBRARY_NAME("i/Import",
										 ILANG_FUNCTION("get", ilang_import_get);
										 ILANG_FUNCTION("check", ilang_import_check);
										 )

}
