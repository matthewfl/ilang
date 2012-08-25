#include "database.h"

#include <iostream>
using namespace std;

namespace ilang {
  Database *System_Database;
  DatabaseFile::DatabaseFile(fs::path path) {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path.c_str(), &db);
    assert(status.ok());
  }
  DatabaseFile::~DatabaseFile() {
    delete db;
  }

  void DatabaseFile::Set(std::string name, storedData *data) {
    
  }
  storedData *DatabaseFile::Get(std::string name) {
    
  }

  class Database_variable : public Variable_modifier {
  private:
    std::string name;
    Variable *var;
  public:
    bool Check(const boost::any &a) { return true; }
    Database_variable(std::string _name, Variable *_var): name(_name), var(_var) {
      cout << "new database variable created " << name << endl;
      System_Database->Get(name);
    }
    void Set(const boost::any &a) {
      
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


  ILANG_VARIABLE_MODIFIER(Db, Database_variable_wrap);
}

