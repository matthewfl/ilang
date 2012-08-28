#include "database.h"
#include "ilang/ilang.h"

#include <string.h>
#include <iostream>
using namespace std;


namespace ilang {
  Database *System_Database;
  DatabaseFile::DatabaseFile(fs::path path) {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path.c_str(), &db);
    cout << "created database " << path << endl;
    assert(status.ok());
  }
  DatabaseFile::~DatabaseFile() {
    delete db;
  }

  void DatabaseFile::Set(std::string name, storedData *data) {
    size_t storedSize = sizeof(storedData);
    if(data->type == storedData::String) storedSize = sizeof(storedData) + data->string_length;
    leveldb::Slice s_data((char*)data, storedSize);
    storedNameRaw NameStr;
    NameStr.type = storedNameRaw::NormalKey;
    size_t nameSize = sizeof(storedNameRaw) - 256 /* for char[256] */ + name.size();
    strncpy(NameStr.name, name.c_str(), 256);
    leveldb::Slice s_name((char*)(&NameStr), nameSize);
    db->Put(leveldb::WriteOptions(), s_name, s_data);
  }
  storedData *DatabaseFile::Get(std::string name) {
    storedNameRaw NameStr;
    NameStr.type = storedNameRaw::NormalKey;
    size_t nameSize = sizeof(storedNameRaw) - 256 /* for char[256] */ + name.size();
    strncpy(NameStr.name, name.c_str(), 256);
    leveldb::Slice s_name((char*)(&NameStr), nameSize);
    std::string value;
    if( db->Get(leveldb::ReadOptions(), s_name, &value).ok() ) {
      char *ret = new char[value.size()+1];
      memcpy(ret, value.data(), value.size());
      return (storedData*)ret;
    }else{
      return NULL;
    }
  }

  void DatabaseFile::setMeta(std::string name, std::string data) {
    name.insert(0,1,1);
    db->Put(leveldb::WriteOptions(), name, data);
  }

  std::string DatabaseFile::getMeta(std::string name) {
    name.insert(0,1,1);
    std::string ret;
    if(! db->Get(leveldb::ReadOptions(), name, &ret).ok() )
      ret.clear(); // idk if this is needed
    return ret;
  }

  class Database_variable : public Variable_modifier {
  private:
    Variable *var;
    std::string name;
    ValuePass toSet;
  public:
    bool Check(const boost::any &a) { return true; }
    Database_variable(std::string _name, Variable *_var): var(_var), name(_name) {
      cout << "new database variable created " << name << endl;
      storedData *dat = System_Database->Get(name);
      if(dat) {
	switch(dat->type) {
	case storedData::IntNumber:
	  toSet = ValuePass(new ilang::Value(dat->Int));
	  break;
	case storedData::FloatNumber:
	  toSet = ValuePass(new ilang::Value(dat->Float));
	  break;
	case storedData::Bool:
	  toSet = ValuePass(new ilang::Value(dat->BoolDat));
	  break;
	case storedData::String:
	  toSet = ValuePass(new ilang::Value(std::string((char*)(dat) + sizeof(storedData), dat->string_length)));
	  break;
	}
	//var->Set(val);
      }
    }
    void Set(const boost::any &a) {
      if(!var) return; // to pervent the system from recursion
      if(toSet) { // replace the default value
	Variable *vvv = var;
	var = NULL;
	vvv->Set(toSet);
	//toSet = NULL;
	var = vvv;
	return;
      }
      if(a.type() == typeid(std::string)) {
	// this is for special cases, most likely string and objects
	string str = boost::any_cast<std::string>(a);
	storedData *dat = (storedData*) new char[sizeof(storedData) + str.size()];
	dat->type = storedData::String;
	dat->string_length = str.size();
	memcpy( ((char*)(dat)) + sizeof(storedData), str.c_str(), str.size() );
	System_Database->Set(name, dat);
	delete dat;
      }else{
	storedData dat;
	if(a.type() == typeid(long)) {
	  dat.type = storedData::IntNumber;
	  dat.Int = boost::any_cast<long>(a);
	}else if(a.type() == typeid(double)) {
	  dat.type = storedData::FloatNumber;
	  dat.Float = boost::any_cast<double>(a);
	}else if(a.type() == typeid(bool)) {
	  dat.type = storedData::Bool;
	  dat.BoolDat = boost::any_cast<bool>(a);
	}
	cout << "setting data for variable " << name << endl;
	System_Database->Set(name, &dat);
      }
    }
  };

  class Database_variable_wrap : public Variable_modifier {
  public:
    bool Check(const boost::any &a) { return true; }
    shared_ptr<Variable_modifier> new_variable(std::string name , Variable *var) {
      assert(System_Database);
      boost::shared_ptr<Variable_modifier> p ( new Database_variable(name, var) );
      return p;
    }
  };


  ILANG_VARIABLE_MODIFIER(DB, Database_variable_wrap);
  ILANG_VARIABLE_MODIFIER(Db, Database_variable_wrap);

  namespace {
    ValuePass DB_metaSet(vector<ValuePass> &args) {
      assert(args.size() == 2);
      assert(args[0]->Get().type() == typeid(string));
      assert(args[1]->Get().type() == typeid(string));
      System_Database->setMeta(boost::any_cast<string>(args[0]->Get()), boost::any_cast<string>(args[1]->Get()));
      return ValuePass(new ilang::Value);
    }
    ValuePass DB_metaGet(vector<ValuePass> &args) {
      assert(args.size() == 1);
      assert(args[0]->Get().type() == typeid(string));
      return ValuePass(new ilang::Value( System_Database->getMeta(boost::any_cast<string>(args[0]->Get())) ));
    }
  }
  ILANG_LIBRARY_NAME("i/db", 
		     ILANG_FUNCTION("metaSet", DB_metaSet)
		     ILANG_FUNCTION("metaGet", DB_metaGet)
		     );
}
