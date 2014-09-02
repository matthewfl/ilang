#ifndef _ilang_import
#define _ilang_import

#include <string>
#include <vector>
#include <list>
#include <map>

#include <boost/filesystem.hpp>

#include "debug.h"
#include "variable_new.h"
//#include "object_new.h"
#include "context.h"

namespace ilang {
//	class Scope;
	//class FileScope;
//	class Object;

	namespace parserNode {
		class Head;
	}

	namespace fs = boost::filesystem;
	class ImportScope;
	extern std::vector<fs::path> ImportSearchPaths;
	extern std::map<fs::path, ImportScope*> ImportedFiles;
	std::map<std::string, ImportScope*> & StaticImportedFiles();

	void Import_Init (int argc, char **argv);

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
		virtual void load(Context &ctx, Handle<Hashable>) {};
		void get(Context &ctx, Handle<Hashable>, fs::path&);
	};
	extern ImportScope GlobalImportScope;

	class ImportScopeFile : public ImportScope {
		// for ilang files
	private:
		//FileScope *m_Scope;
		std::list<std::pair<std::vector<Identifier>, fs::path> > imports;
		parserNode::Head *m_head = NULL;
		Handle<Hashable> GetObject(Context &ctx, std::vector<Identifier>);
		Handle<Hashable> GetObject(Context &ctx, Handle<Hashable>, std::vector<Identifier>&);
		//Handle<Hashable> GetObject(std::vector<Identifier>);
		friend class parserNode::Head;
	public:
		ImportScopeFile(fs::path p);
		void push(std::vector<Identifier> *pre, std::vector<Identifier> *name);
		void resolve(Context &ctx);

		void load(Context &ctx, Handle<Hashable>) override;
		//void provide(FileScope*);
	};

	class ImportScopeC : public ImportScope {
		// for C++ files
	private:
		//char *m_name;
		std::map<std::string, ValuePass> m_members;
	public:
		ImportScopeC(char *);
		explicit ImportScopeC(fs::path p);
		void Set(char *name, ValuePass val);
		void load(Context &ctx, Handle<Hashable>) override;
	};

	Handle<ilang::Object> ImportGetByName(std::string name);
}

#endif // _ilang_import
