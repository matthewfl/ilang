#include "database.h"
#include "ilang.h"
#include "error.h"

#include "database.pb.h"

#include <db_cxx.h>

#include <string.h>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

using namespace std;


namespace ilang {
	Database *System_Database;
	std::string DB_name_prefix;
	std::string DB_name_last;

	class DB_serializer;

	class Database_Object : public Hashable {
	private:
		friend class DB_serializer;
		string name;
	public:
		Database_Object(string n) : name(n) {}
		ValuePass get(Context &ctx, Identifier i) override;
		void set(Context &ctx, Identifier i, ValuePass v) override;
		bool has(Context &ctx, Identifier i) {
			auto v = get(ctx, i);
			return (bool) v;

		}
		Handle<Variable> getVariable(Context &ctx, Identifier i) { assert(0); } // hm...
	};

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
				return valueMaker(entry->integer_dat());
			case Entry::Float:
				return valueMaker(entry->float_dat());
			case Entry::Bool:
				return valueMaker(entry->bool_dat());
			case Entry::String:
				return valueMaker(entry->string_dat());
			case Entry::Object: {
				auto obj = make_handle<Database_Object>(entry->object_id());
				return valueMaker(static_pointer_cast<Hashable>(obj));
			}
			case Entry::Array: {
				ValuePass arr = readStoredData(System_Database->Get(entry->object_id()));
				assert(0);
				// TODO: ehhhhh
				return arr;
			}
			case Entry::Array_contents: {
				assert(0);
				// std::vector<ValuePass> arr_members;
				// arr_members.reserve(entry->array_dat_size());
				// for(int i=0; i < entry->array_dat_size(); i++) {
				//	ValuePass gg = readStoredData(System_Database->Get(entry->array_dat(i)));
				//	 arr_members.push_back(gg);
				// }
				//auto arr = make_handle<ilang::Array>(arr_members);
				//ilang::Array *arr = new ilang::Array(arr_members);
				return valueMaker(true); //arr);
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

		static void createStoredData(Context &ctx, ValuePass a, ilang_db::Entry *entry) {
			using namespace ilang_db;
			if(a->type() == typeid(long)) {
				entry->set_type(ilang_db::Entry::Integer);
				entry->set_integer_dat(a->cast<long>());
			} else if(a->type() == typeid(double)) {
				entry->set_type(Entry::Float);
				entry->set_float_dat(a->cast<double>());
			} else if(a->type() == typeid(bool)) {
				entry->set_type(Entry::Bool);
				entry->set_bool_dat(a->cast<bool>());
			} else if(a->type() == typeid(std::string)) {
				entry->set_type(Entry::String);
				entry->set_string_dat(a->cast<std::string>());
			} else if(a->type() == typeid(Hashable*)) {
				Handle<Hashable> h = a->cast<Hashable*>();
				if(dynamic_pointer_cast<Database_Object>(h)) {
					entry->set_type(Entry::Object);
					entry->set_object_id(dynamic_pointer_cast<Database_Object>(h)->name);
				} else if(dynamic_pointer_cast<Object>(h)) {
					// it is an object
					entry->set_type(Entry::Object);
					std::string n = DB_createName();
					entry->set_object_id(n);
					auto o = dynamic_pointer_cast<Object>(h);
					for(auto it : *o) {
						storedData *ld = createStoredData(ctx, it.second->Get(ctx));
						System_Database->Set(n + it.first.str(), ld);
						delete ld;
					}
					// TODO: support having classes have their own seralizer for going into/out of the db
				} else if(dynamic_pointer_cast<Array>(h)) {
					assert(0);
				} else
					error(0, "type " << a->type().name() << " not allowed in the database\n");
			}
			// rewrite this function
			/*	entry->Clear();
					if(a.type() == typeid(ilang::Object*)) {
					Object *obj = boost::any_cast<ilang::Object*>(a);
					Array *arr;
					if(arr = dynamic_cast<ilang::Array*>(obj)) {
					//assert(0);
					entry->set_type(ilang_db::Entry::Array);
					assert(0);
					/*
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
					assert(0);
					//System_Database->Set(name, dat = createStoredData(arr->members[i]->Get());
					delete dat;
					}
					std::string str_arr_contents;
					arr_contents.SerializeToString(&str_arr_contents);
					System_Database->Set(arr->DB_name, &str_arr_contents);
					}

					entry->set_type(Entry::Array);
					//entry->set_object_id(arr->DB_name);
					}else{
					// need to inspect object and stuff
					// for the time being, we do not want to deal with base classes
					//assert(!obj->baseClass);
					//assert(!obj->C_baseClass);
					/*if(!obj->DB_name) {
					obj->DB_name = DB_createName();
					//assert(obj->members.size() <= 1); // we will say that objs can't have default values for the time being, that will make it so that, the this variable will be ok for the time being
					for(auto it : obj->members) {
					if(it.first == "this") continue;
					string use_name = obj->DB_name;
					use_name += it.first;
					assert(0);
					// TODO:
					//ilang::Variable *v = new Variable(use_name, list<string>({"Db"}));
					//v->Set(it.second->Get());
					delete it.second;
					//it.second = v;
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
					*/
		}

		static storedData * createStoredData(Context &ctx, ValuePass a) {
			ilang_db::Entry entry;
			createStoredData(ctx, a, &entry);
			std::string *ret = new std::string;
			entry.SerializeToString(ret);
			return ret;
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

	ValuePass Database_Object::get(Context &ctx, Identifier i) {
		std::string n = name + i.str();
		storedData *d = System_Database->Get(n);
		if(d)
			return DB_serializer::readStoredData(d);
		assert(0);
		return ValuePass();
	}
	void Database_Object::set(Context &ctx, Identifier i, ValuePass v) {
		std::string n = name + i.str();
		storedData *d = DB_serializer::createStoredData(ctx, v);
		System_Database->Set(n, d);
		delete d;
	}


	// DatabaseFile::DatabaseFile(fs::path path) {
	//	 // leveldb::Options options;
	//	 // options.create_if_missing = true;
	//	 // leveldb::Status status = leveldb::DB::Open(options, path.c_str(), &db);
	//	 // //cout << "created database " << path << endl;
	//	 // assert(status.ok());
	// }
	// DatabaseFile::~DatabaseFile() {
	//	 //delete db;
	// }

	// void DatabaseFile::Set(std::string name, storedData *data) {
	//	 //size_t storedSize = sizeof(storedData);
	//	 //if(data->type == storedData::String || data->type == storedData::Object) storedSize = sizeof(storedData) + data->string_length;
	//	 // leveldb::Slice s_data(*data);
	//	 // storedNameRaw NameStr;
	//	 // NameStr.type = storedNameRaw::NormalKey;
	//	 // size_t nameSize = sizeof(storedNameRaw) - 256 /* for char[256] */ + name.size();
	//	 // strncpy(NameStr.name, name.c_str(), 256);
	//	 // leveldb::Slice s_name((char*)(&NameStr), nameSize);
	//	 // db->Put(leveldb::WriteOptions(), s_name, s_data);
	// }
	// storedData *DatabaseFile::Get(std::string name) {
	//	 // storedNameRaw NameStr;
	//	 // NameStr.type = storedNameRaw::NormalKey;
	//	 // size_t nameSize = sizeof(storedNameRaw) - 256 /* for char[256] */ + name.size();
	//	 // strncpy(NameStr.name, name.c_str(), 256);
	//	 // leveldb::Slice s_name((char*)(&NameStr), nameSize);
	//	 // std::string *value = new std::string;
	//	 // if( db->Get(leveldb::ReadOptions(), s_name, value).ok() ) {
	//	 //		return value;
	//	 // }else{
	//	 //		delete value;
	//	 //		return NULL;
	//	 // }
	//	 return NULL;
	// }

	// void DatabaseFile::setMeta(std::string name, std::string data) {
	//	 // name.insert(0,1,1);
	//	 // db->Put(leveldb::WriteOptions(), name, data);
	// }

	// std::string DatabaseFile::getMeta(std::string name) {
	//	 // name.insert(0,1,1);
	//	 // std::string ret;
	//	 // if(! db->Get(leveldb::ReadOptions(), name, &ret).ok() )
	//	 //		ret.clear(); // idk if this is needed
	//	 // return ret;
	// }


	DatabaseFile::DatabaseFile(fs::path path) {
		db = new Db(NULL, 0);
		cout << "path: " << path.c_str() << endl;
		db->open(NULL,
						 path.c_str(),
						 NULL,
						 DB_BTREE,
						 DB_CREATE,
						 0);
	}

	DatabaseFile::~DatabaseFile() {
		db->close(0);
		delete db;
	}

	void DatabaseFile::Set(std::string name, storedData *data) {
		Dbt key((void*)name.c_str(), name.size());
		Dbt value((void*)data->c_str(), data->size());
		db->put(NULL, &key, &value, DB_OVERWRITE_DUP);
	}

	storedData *DatabaseFile::Get(std::string name) {
		Dbt key((void*)name.c_str(), name.size());
		Dbt value;
		int r = db->get(NULL, &key, &value, 0);
		if(r == DB_NOTFOUND)
			return NULL;
		storedData *ret = new std::string((char*)value.get_data(), value.get_size());
		return ret;
	}

	void DatabaseFile::setMeta(std::string name, std::string data) {
		Set("_meta_data_" + name, &data);
	}

	std::string DatabaseFile::getMeta(std::string name) {
		storedData *ret = Get("_meta_data_" + name);
		if(ret) {
			std::string r(*ret);
			delete ret;
			return r;
		}
		return "";
	}


	void DatabaseDummy::Set(std::string name, storedData *dat) {
		// storedData is typedef to a string type
		_dat[name] = *dat;

	}
	storedData* DatabaseDummy::Get(std::string name) {
		auto it = _dat.find(name);
		if(it == _dat.end())
			return NULL;
		std::string *ret = new std::string;
		*ret = it->second;
		return ret;
	}
	void DatabaseDummy::setMeta(std::string name, std::string dat) {
		_meta[name] = dat;
	}
	std::string DatabaseDummy::getMeta(std::string name) {
		return _meta[name];
	}


	class Database_modifier : public C_Class {
	private:
		std::string name;
		bool inited = false;

		ValuePass check(Context &ctx, Arguments &args) {
			ValuePass v = args[0];
			if(v->type() == typeid(ilang::Function*)) {
				return valueMaker(false);
			}
			if(v->type() == typeid(ilang::Hashable*)) {
				auto h = v->cast<Hashable*>();
				if(dynamic_pointer_cast<Class>(h)) { return valueMaker(false); }
				if(dynamic_pointer_cast<Class_instance>(h)) { return valueMaker(false); }
			}
			return valueMaker(true);
		}
		ValuePass setting(Context &ctx, Arguments &args) {
			if(!inited) {
				inited = true;
				storedData *dat = System_Database->Get(name);
				if(dat) {
					// if the value already exists in the db, ignore the first attempt to set it,
					// as that is considered the default value
					delete dat;
					return ValuePass();
				}
			}
			storedData *dat = DB_serializer::createStoredData(ctx, args[0]);
			System_Database->Set(name, dat);
			delete dat;
			return ValuePass();
		}

		ValuePass getting(Context &ctx, Arguments &args) {
			storedData *dat = System_Database->Get(name);
			if(dat) {
				ValuePass ret = DB_serializer::readStoredData(dat);
				delete dat;
				return ret;
			}
			return ValuePass();
		}

		ValuePass new_call(Context &ctx, Arguments &args) {
			Handle<Hashable> h(this);
			return valueMaker(h);
		}

		void Init() {
			reg("check", &Database_modifier::check);
			reg("setting", &Database_modifier::setting);
			reg("getting", &Database_modifier::getting);
			reg("new", &Database_modifier::new_call);
		}
	public:
		Database_modifier(Context &ctx, Arguments &args) {
			args.inject(name);
			Init();
		}
	};

	void Database_register_function (Context &ctx, Arguments &args, ValuePass *ret) {
		auto d = make_handle<Database_modifier>(ctx, args);
		*ret = valueMaker(static_pointer_cast<Hashable>(d));
	}

	struct Database_modifier_register {
		Database_modifier_register() {
			//global_scope_register("DB", ilang::Class_Creater<Database_modifier>());
			ilang::Function regf(Database_register_function);
			global_scope_register("DB", valueMaker(regf));
		}
	} __database_modifier_register;

	// I am not sure if I want the system to be able to access the meta data that is held, but for the time being I guess this is ok
	// if programmers couldn't break it, what fun would there be
	namespace {
		ValuePass DB_metaSet(Context &ctx, Arguments &args) {
			error(args.size() == 2, "db.metaSet takes 2 arguments");
			error(args[0]->type() == typeid(string), "First argument to db.metaSet should be a string");
			error(args[1]->type() == typeid(string), "Second argument to db.metaSet should be a string");
			System_Database->setMeta(args[0]->cast<string>(), args[1]->cast<string>());
			return ValuePass();
		}
		ValuePass DB_metaGet(Context &ctx, Arguments &args) {
			error(args.size() == 1, "db.metaGet takes 1 argument");
			error(args[0]->type() == typeid(string), "First argument to db.metaGet must should be a string");
			return valueMaker(System_Database->getMeta(args[0]->cast<string>()));
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
#define ggg(tt) (pt.get_value_optional< tt >() != boost::optional< tt >())
			cerr << ggg(bool) << ggg(long) << ggg(int) << ggg(double) << ggg(float) << ggg(std::string) << endl;
			cerr << "\""<< pt.data()<< "\"";
#define load_type(type, db_name, db_type)										\
			try {																									\
				entry.set_##db_name##_dat(pt.get_value< type >());	\
					entry.set_type( ilang_db::Entry::db_type );				\
					return;																						\
			} catch (boost::property_tree::ptree_bad_data e) {		\
																														\
			}


			// TODO: something better, as atm the int 1 is parsed as a bool
			// this means that bools will become ints as either 0 or 1
			//load_type(bool, bool, Bool);

			load_type(long, integer, Integer);
			load_type(int, integer, Integer);
			load_type(double, float, Float);
			load_type(float, float, Float);
			load_type(std::string, string, String);
			assert(0); // wtf
#undef load_type
			// if(pt.get_value_optional<bool>() != boost::optional<bool>()) {
			//	entry.set_type(ilang_db::Entry::Bool);
			//	entry.set_bool_dat(pt.get_value<bool>());
			// } else if(pt.get_value_optional<long>() != boost::optional<long>()) {
			//	entry.set_type(ilang_db::Entry::Integer);
			//	entry.set_integer_dat(pt.get_value<long>());
			// }else if(pt.get_value_optional<double>() != boost::optional<double>()) {
			//	entry.set_type(ilang_db::Entry::Float);
			//	entry.set_float_dat(pt.get_value<double>());
			// }else if(pt.get_value_optional<std::string>() != boost::optional<std::string>()) {
			//	entry.set_type(ilang_db::Entry::String);
			//	entry.set_string_dat(pt.get_value<std::string>());
			// }else{
			//	assert(0); // wtf
			// }
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
				//	 cerr << ",";
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
		// TODO:
		//value->toJSON(ss);
		cout << ss;

	}
}
