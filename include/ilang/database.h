#ifndef _ilang_database
#define _ilang_databsae

#include "debug.h"
#include "variable.h"

#include "leveldb/db.h"

#include <boost/filesystem.hpp>

/* notes on the database
 * When networked: (raw storage, trust all nodes)
 *  1 byte : type of value
 *           0: error
 *           1: reserved for the system to contain local meta data
 *           2: R/W all, contains data 
 *  4 bytes: version of hash ring function
 *  8 bytes: value of hash ring
 *  N bytes: variable number of bytes representing the 'key'
 *
 * When networked: (raw storage, trust few nodes)
 *  1 byte : type of value
 *           0: error
 *           1: reserved for the system to contain local meta data
 *           2: R/W all, contains data
 *           3: R-all W-one, contains data (techincally all can read all the data, but when it is encrypted then only some will understand it)
 *           4: 10 bytes containing id of key, signature generated 
 *           5: seperate secion that contains the public keys by id assigned, also signed using some global master-ish key to have this id and a date that it expires
 *  4 bytes: version of hash ring function
 *  8 bytes: value of hash ring
 *  N bytes: variable number of bytes representing the 'key'
 * 
 * 
 * When local:
 *  1 byte : type of value
 *           0: error
 *           1: reserved for system to contain local meta data
 *           2: R/W all, contains data
 *  N bytes: variable number of bytes representing the 'key'
 */

namespace ilang {
  namespace fs = boost::filesystem;
  struct storedData {
    enum {
      IntNumber = 1,
      FloatNumber = 2,
      Bool = 3,
      String = 4,
      Object = 5
    } type;
    union {
      long Int;
      double Float;
      bool BoolDat;
      size_t string_length;
    };
    // if the data is a string type, then just have the value follow this struct
  };
  struct storedNameRaw {
    enum {
      metaData = 1,
      NormalKey = 2
    } type;
    char name[256];
  };
  class Database {
  public:
    virtual void Set(std::string, storedData*)=0;
    virtual storedData *Get(std::string)=0;
    
    virtual void setMeta(std::string, std::string)=0;
    virtual std::string getMeta(std::string)=0;

    virtual ~Database() {};
  };
  class DatabaseFile : public Database {
  protected:
    leveldb::DB *db;
  public:
    DatabaseFile (fs::path);
    ~DatabaseFile();
    void Set(std::string name, storedData *data);
    storedData *Get(std::string name);
    
    void setMeta(std::string name, std::string data);
    std::string getMeta(std::string name);
  };
  extern Database *System_Database;
}

#endif // _ilang_databsae
