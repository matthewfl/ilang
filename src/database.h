#ifndef _ilang_database
#define _ilang_databsae

#include "debug.h"
#include "variable_new.h"

#include <boost/filesystem.hpp>
#include <stdio.h>
#include <map>

/* notes on the database
 * When networked: (raw storage, trust all nodes)
 *	1 byte : type of value
 *			 0: error
 *			 1: reserved for the system to contain local meta data
 *			 2: R/W all, contains data
 *	4 bytes: version of hash ring function
 *	8 bytes: value of hash ring
 *	N bytes: variable number of bytes representing the 'key'
 *
 * When networked: (raw storage, trust few nodes)
 *	1 byte : type of value
 *			 0: error
 *			 1: reserved for the system to contain local meta data
 *			 2: R/W all, contains data
 *			 3: R-all W-one, contains data (techincally all can read all the data, but when it is encrypted then only some will understand it)
 *			 4: 10 bytes containing id of key, signature generated
 *			 5: seperate secion that contains the public keys by id assigned, also signed using some global master-ish key to have this id and a date that it expires
 *	4 bytes: version of hash ring function
 *	8 bytes: value of hash ring
 *	N bytes: variable number of bytes representing the 'key'
 *
 *
 * When local:
 *	1 byte : type of value
 *			 0: error
 *			 1: reserved for system to contain local meta data
 *			 2: R/W all, contains data
 *	N bytes: variable number of bytes representing the 'key'
 *
 *
 * Name prefixes:
 * N: Named variable
 * O: Object member
 */

namespace ilang {
	namespace fs = boost::filesystem;

	// using protobuf for storage format, see database.proto
	typedef std::string storedData;

	struct storedNameRaw {
		enum {
			metaData = 1,
			NormalKey = 2
		} type;
		char name[256];
	};
	// creates a ValuePass with the correct value when given a poitner to a storedData
	// does not delete the storedData
	//ValuePass DB_readStoredData (storedData*);
	// creats a storedData from a boost::any, returned value needs to be delete by calling function
	//storedData *DB_createStoredData (const boost::any&);
	class DB_serializer;

	char *DB_createName();

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
		//leveldb::DB *db;
	public:
		DatabaseFile (fs::path);
		~DatabaseFile();
		void Set(std::string name, storedData *data);
		storedData *Get(std::string name);

		void setMeta(std::string name, std::string data);
		std::string getMeta(std::string name);
	};
	class DatabaseDummy : public Database {
	public:
		std::map<std::string, std::string> _dat;
		std::map<std::string, std::string> _meta;
		void Set(std::string, storedData*);
		storedData *Get(std::string);
		void setMeta(std::string, std::string);
		std::string getMeta(std::string);
	};
	int DatabaseLoad(std::string, FILE*);
	int DatabaseDump(std::string, FILE*);
	extern Database *System_Database;
}

#endif // _ilang_databsae
