#include "database.h"
#include "ilang/ilang.h"
#include "error.h"

#include <string.h>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;


namespace ilang {
  Database *System_Database;
  std::string DB_name_prefix;
  std::string DB_name_last;

  ValuePass DB_readStoredData (storedData *dat) {
    ilang::Object *obj; // here to make the compiler happy
    ilang::Array *arr;
    switch(dat->type) {
    case storedData::IntNumber:
      return ValuePass(new ilang::Value(dat->Int));
    case storedData::FloatNumber:
      return ValuePass(new ilang::Value(dat->Float));
    case storedData::Bool:
      return ValuePass(new ilang::Value(dat->BoolDat));
    case storedData::String:
      return ValuePass(new ilang::Value(std::string((char*)(dat) + sizeof(storedData), dat->string_length)));
    case storedData::Object:
      obj = new ilang::Object;
      obj->DB_name = new char[dat->string_length];
      memcpy(obj->DB_name, ((char*)(dat)) + sizeof(storedData), dat->string_length);
      return ValuePass(new ilang::Value(obj));
    case storedData::Array:
      assert(0);
      arr = new ilang::Array;
      arr->DB_name = new char[dat->string_length];
      // the start and end places are going to have to be set up so they are kept in other variables so that they can be easily changed when the array is resized
      //arr->DB_start_index = dat->Array_start;
      //arr->DB_end_index = dat->Array_end;
      memcpy(obj->DB_name, ((char*)(dat)) + sizeof(storedData), dat->string_length);

    default:
      assert(0);
    }
  }

  storedData * DB_createStoredData (const boost::any &a) {
    storedData *dat;
    // these special cases need to have extra stuff in the Set command so that the system knows the correct length of the data
    if(a.type() == typeid(std::string)) {
      // this is for special cases, most likely string and objects
      string str = boost::any_cast<std::string>(a);
      dat = (storedData*) new char[sizeof(storedData) + str.size()];
      dat->type = storedData::String;
      dat->string_length = str.size();
      memcpy( ((char*)(dat)) + sizeof(storedData), str.c_str(), str.size() );
    }else if(a.type() == typeid(ilang::Object*)) {
      Object *obj = boost::any_cast<ilang::Object*>(a);
      Array *arr;
      // obj can be of an array type that we want to store differently
      if(arr = dynamic_cast<ilang::Array*>(obj)) {
	assert(0);
	// might change the array to to be unable to random inserts/removes in the middle when it is saved inside the database
	// this way arrays will not become overly stressful on the database, as the way that I was thinking of implemting it
	// it would require that the database change all the keys of the other elements when something is removed from the center

      }else{
	// for the time being, we do not want to deal with base classes
	assert(!obj->baseClass);
	assert(!obj->C_baseClass);
	if(!obj->DB_name) {
	  obj->DB_name = DB_createName();
	  //assert(obj->members.size() <= 1); // we will say that objs can't have default values for the time being, that will make it so that, the this variable will be ok for the time being
	  for(auto it : obj->members) {
	    if(it.first == "this") continue;
	    string use_name = obj->DB_name;
	    use_name += it.first;
	    ilang::Variable *v = new Variable(use_name, list<string>({"Db"}));
	    v->Set(it.second->Get());
	    delete it.second;
	    it.second = v;
	  }
	  //obj->members.clear();
	  //cout << "db name given " << obj->DB_name << endl;
	}
	dat = (storedData*) new char[sizeof(storedData) + strlen(obj->DB_name)];
	dat->type = storedData::Object;
	dat->string_length = strlen(obj->DB_name) +1;
	memcpy( ((char*)(dat)) + sizeof(storedData), obj->DB_name, dat->string_length );
	cout << "length: " << dat->string_length << " " << ((char*)(dat)) + sizeof(storedData) << endl;
      }
    }else{
      dat = new storedData;
      if(a.type() == typeid(long)) {
	dat->type = storedData::IntNumber;
	dat->Int = boost::any_cast<long>(a);
      }else if(a.type() == typeid(double)) {
	dat->type = storedData::FloatNumber;
	dat->Float = boost::any_cast<double>(a);
      }else if(a.type() == typeid(bool)) {
	dat->type = storedData::Bool;
	dat->BoolDat = boost::any_cast<bool>(a);
      }else if(a.type() == typeid(ilang::Class*)) {
	assert(0);
	// for the moment, classes will not be saveable in the db
      }else if(a.type() == typeid(ilang::Function)) {
	assert(0);

      }else{
	assert(0);
      }
    }
    return dat;
  }


  char * DB_createName () {
    // first char must be -, so that the name would be an ilegal variable name
    if(DB_name_prefix.empty()) {
      DB_name_prefix = "-local-db";
    }
    if(DB_name_last.empty()) {
      assert(System_Database);
      DB_name_last = System_Database->getMeta("Object_counter");
      if(DB_name_last.empty()) {
	DB_name_last = "-A";
      }
    }
    int pos=1, len=DB_name_last.size();
    while(true) {
      if(DB_name_last[pos] > 250) {
	DB_name_last[pos] = (char)1;
	pos++;
	continue;
      }else{
	DB_name_last[pos]++;
	if(DB_name_last[pos] == '-')
	  DB_name_last[pos]++;
	break;
      }
    }
    System_Database->setMeta("Object_counter", DB_name_last);
    //cout << "name: " << DB_name_prefix << " " << DB_name_last << endl << flush;
    std::string str = DB_name_prefix;
    str += DB_name_last;
    str += "-";
    char *ret = new char[str.size()];
    strcpy(ret, str.c_str());
    return ret;
  }



  DatabaseFile::DatabaseFile(fs::path path) {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path.c_str(), &db);
    //cout << "created database " << path << endl;
    assert(status.ok());
  }
  DatabaseFile::~DatabaseFile() {
    delete db;
  }

  void DatabaseFile::Set(std::string name, storedData *data) {
    size_t storedSize = sizeof(storedData);
    if(data->type == storedData::String || data->type == storedData::Object) storedSize = sizeof(storedData) + data->string_length;
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
    //Variable *var;
    bool recursion_block;
    std::string name;
    ValuePass toSet;
    bool hasRead;
  public:
    bool Check(Variable *self, const boost::any &a) {
      // should block types that are not allowed in the database
      // return a.type() != typeid(ilang::Class*) && a.type() != typeid(ilang::Function_ptr);
      return true;
    }
    Database_variable(Variable *var, std::string _name): name(_name), hasRead(false), recursion_block(false) {
      //cout << "new database variable created " << name << endl;
      storedData *dat = System_Database->Get(name);
      if(dat) {
	toSet = DB_readStoredData(dat);
	delete dat;
	//_var->Set(toSet);
      }
    }
    void Set(Variable *var, const boost::any &a) {
      //cout << "setting data for variable " << name << endl;
      if(recursion_block) return; // to pervent the system from recursion
      //if(!hasRead) return;
      if(toSet) { // replace the default value
	//cout << "setting defualt value for " << name << endl;
	if(!hasRead) {
	  //Variable *vvv = var;
	  //var = NULL;
	  recursion_block = true;
	  var->Set(toSet);
	  recursion_block = false;
	  //var = vvv;
	}
	toSet = ValuePass();
	if(!hasRead) return;
      }
      storedData *dat = DB_createStoredData(a);
      System_Database->Set(name, dat);
      delete dat;
    }
    void Read(Variable *var, ValuePass &val) {
      /*storedData *dat = System_Database->Get(name);
      if(dat) {
	val = DB_readStoredData(dat);
	delete dat;
	}*/
      if(toSet)
	val = toSet;
      hasRead = true;
    }
  };

  class Database_variable_wrap : public Variable_modifier {
  public:
    bool Check(Variable *self, const boost::any &a) { return true; }
    shared_ptr<Variable_modifier> new_variable(Variable *self, std::string name) {
      assert(System_Database);
      boost::shared_ptr<Variable_modifier> p ( new Database_variable(self, name) );
      return p;
    }
  };


  ILANG_VARIABLE_MODIFIER(DB, Database_variable_wrap);
  ILANG_VARIABLE_MODIFIER(Db, Database_variable_wrap);

  // I am not sure if I want the system to be able to access the meta data that is held, but for the time being I guess this is ok
  // if programmers couldn't break it, what fun would there be
  namespace {
    ValuePass DB_metaSet(vector<ValuePass> &args) {
      error(args.size() == 2, "db.metaSet takes 2 arguments");
      error(args[0]->Get().type() == typeid(string), "First argument to db.metaSet should be a string");
      error(args[1]->Get().type() == typeid(string), "Second argument to db.metaSet should be a string");
      System_Database->setMeta(boost::any_cast<string>(args[0]->Get()), boost::any_cast<string>(args[1]->Get()));
      return ValuePass(new ilang::Value);
    }
    ValuePass DB_metaGet(vector<ValuePass> &args) {
      error(args.size() == 1, "db.metaGet takes 1 argument");
      error(args[0]->Get().type() == typeid(string), "First argument to db.metaGet must should be a string");
      return ValuePass(new ilang::Value( System_Database->getMeta(boost::any_cast<string>(args[0]->Get())) ));
    }
  }
  ILANG_LIBRARY_NAME("i/db",
		     ILANG_FUNCTION("metaSet", DB_metaSet)
		     ILANG_FUNCTION("metaGet", DB_metaGet)
		     );

}

// for loading json data into and out of the database
namespace ilang {
  using namespace boost::property_tree;
  int DatabaseLoad(FILE *file) {
    char buffer[256];
    //std::string str;
    //boost::property_tree::basic
    //basic_ptree tree;
    stringstream str;
    ptree tree;

    while( fgets(buffer, 256, file) != NULL ) {
      str << buffer;
    }
    read_json(str, tree);

  }
  int DatabaseDump(FILE *file) {

  }
}
