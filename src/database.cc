#include "database.h"
#include "ilang.h"
#include "error.h"

#include "database.pb.h"

#include <string.h>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;


namespace ilang {
  Database *System_Database;
  std::string DB_name_prefix;
  std::string DB_name_last;

  class DB_serializer {
    // this is a dummy class to make friends with other data types
    // so we can access there internals
  private:
    DB_serializer() {};
  public:

    static ValuePass readStoredData(ilang_db::Entry *entry) {
      using namespace ilang_db;
      switch(entry->type()) {
      case Entry::Integer:
	return ValuePass(new ilang::Value(entry->integer_dat()));
      case Entry::Float:
	return ValuePass(new ilang::Value(entry->float_dat()));
      case Entry::Bool:
	return ValuePass(new ilang::Value(entry->bool_dat()));
      case Entry::String:
	return ValuePass(new ilang::Value(entry->string_dat()));
      case Entry::Object: {
	ilang::Object *obj = new ilang::Object;
	obj->DB_name = new char[entry->object_id().size()+1];
	memcpy(obj->DB_name, entry->object_id().c_str(), entry->object_id().size()+1);
	return ValuePass(new ilang::Value(obj));
      }
      case Entry::Array: {
	ValuePass arr = readStoredData(System_Database->Get(entry->object_id()));
	ilang::Array *a = static_cast<ilang::Array*>(boost::any_cast<ilang::Object*>(arr->Get()));
	a->DB_name = new char[entry->object_id().size()+1];
	memcpy(a->DB_name, entry->object_id().c_str(), entry->object_id().size()+1);
	return arr;
      }
      case Entry::Array_contents: {
	std::vector<ValuePass> arr_members;
	arr_members.reserve(entry->array_dat_size());
	for(int i=0; i < entry->array_dat_size(); i++) {
	  ValuePass gg = readStoredData(System_Database->Get(entry->array_dat(i)));
	  arr_members.push_back(gg);
	}
	ilang::Array *arr = new ilang::Array(arr_members);
	return ValuePass(new ilang::Value(static_cast<ilang::Object*>(arr)));
      }
      default:
	assert(0);
      }
    }


    static ValuePass readStoredData (storedData *dat) {
      ilang_db::Entry entry;
      entry.ParseFromString(*dat);
      return readStoredData(&entry);
    }

    static void createStoredData(const boost::any &a, ilang_db::Entry *entry) {
      using namespace ilang_db;
      entry->Clear();
      if(a.type() == typeid(ilang::Object*)) {
	Object *obj = boost::any_cast<ilang::Object*>(a);
	Array *arr;
	if(arr = dynamic_cast<ilang::Array*>(obj)) {
	  //assert(0);
	  entry->set_type(ilang_db::Entry::Array);
	  if(!arr->DB_name) {
	    Entry arr_contents;
	    arr_contents.set_type(Entry::Array_contents);
	    arr->DB_name = DB_createName();
	    for(int i=0; i < arr->members.size(); i++) {
	      //storedData *dat = DB_createStoredData(arr->members[i]->Get());
	      //entry.

	      // Entry *arr_ent = entry->add_array_dat();
	      // createStoredData(arr->members[i]->Get(), arr_ent);

	      char *name = DB_createName();
	      arr_contents.add_array_dat(name);
	      storedData *dat;
	      System_Database->Set(name, dat = createStoredData(arr->members[i]->Get()->Get()));
	      delete dat;
	    }
	    std::string str_arr_contents;
	    arr_contents.SerializeToString(&str_arr_contents);
	    System_Database->Set(arr->DB_name, &str_arr_contents);
	  }
	  entry->set_type(Entry::Array);
	  entry->set_object_id(arr->DB_name);
	}else{
	  // need to inspect object and stuff
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
	  //dat = (storedData*) new char[sizeof(storedData) + strlen(obj->DB_name)];
	  //dat->type = storedData::Object;
	  //dat->string_length = strlen(obj->DB_name) +1;
	  //memcpy( ((char*)(dat)) + sizeof(storedData), obj->DB_name, dat->string_length );
	  //cout << "length: " << dat->string_length << " " << ((char*)(dat)) + sizeof(storedData) << endl;
	  entry->set_type(Entry::Object);
	  entry->set_object_id(obj->DB_name);
	}
      }else if(a.type() == typeid(std::string)) {
	entry->set_type(ilang_db::Entry::String);
	entry->set_string_dat(boost::any_cast<std::string>(a));
      }else if(a.type() == typeid(long)) {
	entry->set_type(ilang_db::Entry::Integer);
	entry->set_integer_dat(boost::any_cast<long>(a));
      }else if(a.type() == typeid(double)) {
	entry->set_type(ilang_db::Entry::Float);
	entry->set_float_dat(boost::any_cast<double>(a));
      }else if(a.type() == typeid(bool)) {
	entry->set_type(ilang_db::Entry::Bool);
	entry->set_bool_dat(boost::any_cast<bool>(a));
      }else if(a.type() == typeid(ilang::Class*)) {
	assert(0);
	// no classes in the db
      }else{
	cout << a.type().name() << endl;
	assert(0);
	// have no idea what this is
      }
    }

    static storedData * createStoredData(const boost::any &a) {
      ilang_db::Entry entry;
      createStoredData(a, &entry);
      std::string *ret = new std::string;
      entry.SerializeToString(ret);
      return ret;
    }

    static void refreshStoredData(const boost::any &a) {
      using namespace ilang_db;
      if(a.type() == typeid(ilang::Array*)) {
	// TODO: fix to reuse names
	ilang::Array *arr = boost::any_cast<ilang::Array*>(a);
	assert(arr->DB_name);
	Entry arr_contents;
	arr_contents.set_type(Entry::Array_contents);

	for(int i=0; i < arr->members.size(); i++) {
	  char *name = DB_createName();
	  arr_contents.add_array_dat(name);
	  storedData *dat;
	  System_Database->Set(name, dat = createStoredData(arr->members[i]->Get()->Get()));
	  delete dat;
	}
	std::string str_arr_contents;
	arr_contents.SerializeToString(&str_arr_contents);
	System_Database->Set(arr->DB_name, &str_arr_contents);
      }else{
	// nothing else should use this at this time
	assert(0);
      }
    }
  };

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
    //size_t storedSize = sizeof(storedData);
    //if(data->type == storedData::String || data->type == storedData::Object) storedSize = sizeof(storedData) + data->string_length;
    leveldb::Slice s_data(*data);
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
    std::string *value = new std::string;
    if( db->Get(leveldb::ReadOptions(), s_name, value).ok() ) {
      return value;
    }else{
      delete value;
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
	toSet = DB_serializer::readStoredData(dat);
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
      storedData *dat = DB_serializer::createStoredData(a);

      System_Database->Set(name, dat);
      delete dat;
    }
    void Read(Variable *var, ValuePass &val) {
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
  ILANG_VARIABLE_MODIFIER(Database, Database_variable_wrap);

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
  /*
  void createStoredDataFromJSON(ptree tree, ilang_db::Entry &entry) {
    cout << 'z' << tree.size();
    if(tree.get_value_optional<int>() != boost::optional<int>()) {
      cout << 'a';
    }else if(tree.get_value_optional<long>() != boost::optional<long>()) {
      cout << 'b';
    }else if(tree.get_value_optional<double>() != boost::optional<double>()) {
      cout << 'c';
    }else if(tree.get_value_optional<float>() != boost::optional<float>()) {
      cout << 'd';
    }
    // else if(tree.get_value_optional<std::string>() != boost::optional<std::string>()) {
    //	 cout << 'e';
    //	 cout << tree.get_value_optional<std::string>() << endl;
    // }
    else{
      cout << 'f';

      for(auto it : tree) {
	cout << it.first << typeid(it.first).name() << it.first.empty() << endl ;
	createStoredDataFromJSON(it.second, entry);
	//createStoredDataFromJSON
      }
    }

  }
  */

  void loadTree (ptree &pt, ilang_db::Entry &entry) {
    if (pt.empty()) {
      //#define ggg(tt) (pt.get_value_optional< tt >() != boost::optional< tt >())
      //cerr << ggg(long) << ggg(int) << ggg(double) << ggg(float) << ggg(std::string) << endl;
      //cerr << "\""<< pt.data()<< "\"";
      if(pt.get_value_optional<bool>() != boost::optional<bool>()) {
	entry.set_type(ilang_db::Entry::Bool);
	entry.set_bool_dat(pt.get_value<bool>());
      } else if(pt.get_value_optional<long>() != boost::optional<long>()) {
	entry.set_type(ilang_db::Entry::Integer);
	entry.set_integer_dat(pt.get_value<long>());
      }else if(pt.get_value_optional<double>() != boost::optional<double>()) {
	entry.set_type(ilang_db::Entry::Float);
	entry.set_float_dat(pt.get_value<double>());
      }else if(pt.get_value_optional<std::string>() != boost::optional<std::string>()) {
	entry.set_type(ilang_db::Entry::String);
	entry.set_string_dat(pt.get_value<std::string>());
      }else{
	assert(0); // wtf
      }
    } else {
      //if (level) cerr << endl;
      bool arr = pt.begin()->first.empty();
      entry.set_type(arr ? ilang_db::Entry::Array : ilang_db::Entry::Object);
      char *name = DB_createName();
      entry.set_object_id(name);
      ilang_db::Entry arr_contents;
      arr_contents.set_type(ilang_db::Entry::Array_contents);
      // if there is no first element, then what? that makes this an empty object? as we don't have null
      //cerr << arr << "{" << endl;
      for (ptree::iterator pos = pt.begin(); pos != pt.end(); pos++) {
	ilang_db::Entry sub_entry;
	std::string dat;
	if(arr) {
	  // TODO: check that this only happens for arrays
	  char *sub_name = DB_createName();
	  arr_contents.add_array_dat(sub_name);
	  loadTree(pos->second, sub_entry);
	  sub_entry.SerializeToString(&dat);
	  System_Database->Set(sub_name, &dat);
	  delete sub_name;
	}else{
	  string sub_name = name;
	  sub_name += pos->first;
	  loadTree(pos->second, sub_entry);
	  sub_entry.SerializeToString(&dat);
	  System_Database->Set(sub_name, &dat);
	}
	//cerr << "\"" << pos->first << "\": ";
	//loadTree(pos->second, sub_entry);//, level + 1);
	// ++pos;
	// if (pos != pt.end()) {
	//   cerr << ",";
	// }
	//cerr << endl;
      }
      if(arr) {
	std::string dat;
	arr_contents.SerializeToString(&dat);
	System_Database->Set(name, &dat);
      }

      //cerr << " }";
      delete name; // should change to std::string ?
    }
  }


  int DatabaseLoad(string name, FILE *file) {
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

    // for(auto it : tree) {
    //	 cout << it.first << ' ' << it.second.data() << endl;
    // }

    ilang_db::Entry entry;

    loadTree(tree, entry);

    std::string dat;
    entry.SerializeToString(&dat);
    System_Database->Set(name, &dat);
    // ilang_db::Entry entry;
    // createStoredDataFromJSON(tree, entry);


  }

  int DatabaseDump(string name, FILE *file) {
    storedData *data = ilang::System_Database->Get(name);
    assert(data);
    ValuePass value = DB_serializer::readStoredData(data);
    std::stringstream ss;
    value->toJSON(ss);
    cout << ss;

  }
}


namespace ilang {
  void Array::RefreshDB() {
    if(!DB_name) return;

    using namespace ilang_db;
    Entry arr_contents;
    arr_contents.set_type(Entry::Array_contents);
    for(int i=0; i < members.size(); i++) {
      char *name = DB_createName();
      arr_contents.add_array_dat(name);
      storedData *dat;
      System_Database->Set(name, dat =	DB_serializer::createStoredData(members[i]->Get()->Get()));
      delete dat;
    }
    std::string str_arr_contents;
    arr_contents.SerializeToString(&str_arr_contents);
    System_Database->Set(DB_name, &str_arr_contents);
  }
}
