// this is a very bad hack to make this work, in the future I would like to be able to have some sort of visitor system that allows for this to be done in a clean way.

#define private public
#include "object.h"
#undef private

#include "ilang.h"
#include "error.h"

#include <map>
#include <string>

namespace {
	using namespace std;
	using namespace ilang;

	class Mapper : public ilang::C_Class {
	private:
		ValuePass m_objectHold;
		ilang::Object *m_obj;
		std::map<std::string, std::vector<ValuePass> > m_emitted;

		ValuePass EmitFunct(std::vector<ValuePass> &args) {
			error(args.size() == 2, "map.emit expects 2 arguments");
			error(args[0]->Get().type() == typeid(std::string), "map.emit expects the first argument to be a string");

			m_emitted[boost::any_cast<std::string>(args[0]->Get())].push_back(args[1]); // save the emitted element into the map, this will not scale well at the moment.

			return ValuePass(new ilang::Value(true));
		}

		ValuePass MapAction(std::vector<ValuePass> &args) {
			error(args.size() == 1, "map.map expects 1 argument");
			error(args[0]->Get().type() == typeid(ilang::Function), "map.map expects a function");

			ilang::Function funct = boost::any_cast<ilang::Function>(args[0]->Get());

			ScopePass obj_scope = ScopePass();
			if(funct.object) {
				obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(funct.object->Get())));
			}

			ValuePass ret = ValuePass(new ilang::Value);

			Mapper *returnMapper = new Mapper();
			ilang::Function emitFunct;
			emitFunct.native = true;
			emitFunct.ptr = [returnMapper](ScopePass scope, std::vector<ValuePass> &args, ValuePass *ret) {
				*ret = returnMapper->EmitFunct(args);
				assert(*ret);
			};
			ValuePass emitFunctVal = ValuePass(new ilang::Value(emitFunct));

#define CALL_WITH(_key, _value)																					\
			{																																	\
				std::vector<ValuePass> params =																	\
					{ValuePass(new ilang::Value( _key )), _value , emitFunctVal};	\
				funct.ptr(obj_scope, params, &ret);															\
			}

			if(m_obj->baseClass) {
				// this object was constructed off some ilang base class
			}else if(m_obj->C_baseClass) {
				// this object was constructed off some C++ base class
			}else if(m_obj->DB_name) {
				// then the object is located in the database are we are going to need to branch out and figure out where this
			}else{
				// put in an if statement so if the object is large that it will use threads when dealing with the object
				// this this is just a normal object with all the elements in the object
				for(auto vals : m_obj->members) {
					if(vals.first == "this") continue;
					ilang::Variable *var = vals.second;
					CALL_WITH(vals.first, var->Get());
				}
			}

#undef CALL_WITH


			return ValuePass(new ilang::Value(new ilang::Object(returnMapper)));
		}

		ValuePass Get(std::vector<ValuePass> &args) {
			error(args.size() == 1, "map.get expects 1 arugment");
			error(args[0]->Get().type() == typeid(std::string), "map.get expects a string");

			return ValuePass(new ilang::Value((ilang::Object*) new ilang::Array(m_emitted[boost::any_cast<std::string>(args[0]->Get())])));
		}

		ValuePass Reduce(std::vector<ValuePass> &args) {
			error(args.size() == 1, "map.reduce expects 1 argument");
			error(args[0]->Get().type() == typeid(ilang::Function), "map.reduce expects a function");
			error(!m_obj, "can not reduce directly on an object"); // TODO: remove this constrain

			ilang::Function funct = boost::any_cast<ilang::Function>(args[0]->Get());

			ScopePass obj_scope = ScopePass();
			if(funct.object) {
				obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(funct.object->Get())));
			}

			Mapper *returnMapper = new Mapper();
			ilang::Function emitFunct;
			emitFunct.native = true;
			emitFunct.ptr = [returnMapper](ScopePass scope, std::vector<ValuePass> &args, ValuePass *ret) {
				*ret = returnMapper->EmitFunct(args);
				assert(*ret);
			};
			ValuePass emitFunctVal = ValuePass(new ilang::Value(emitFunct));

#define CALL_WITH(_key, _value)																					\
			{																																	\
				std::vector<ValuePass> params =																	\
					{ValuePass(new ilang::Value( _key )), _value , emitFunctVal};	\
				funct.ptr(obj_scope, params, &ret);															\
			}

			ValuePass ret = ValuePass(new ilang::Value);

			for(auto vals : m_emitted) {
				ValuePass arr = ValuePass(new ilang::Value((ilang::Object*) new ilang::Array(vals.second)));
				CALL_WITH(vals.first, arr);
			}

#undef CALL_WITH
		}

		void Init() {
			reg("map", &Mapper::MapAction);
			reg("get", &Mapper::Get);
			reg("reduce", &Mapper::Reduce);
		}

	public:
		Mapper(ValuePass _hold, ilang::Object *_obj) :
			m_objectHold(_hold), m_obj(_obj)
		{
			assert(m_obj);
			Init();
		}
		Mapper() : m_obj(NULL) {
			Init();
		}
	};

	ValuePass createMapper(std::vector<ValuePass> &args) {
		// when the argument is of a class that has not been constructed, then the type is ilang::Class*, after the obhject is constructed then the type is ilang::Object* with a pointer to class
		error(args.size() == 1, "map.create expects 1 argument");
		error(args[0]->Get().type() == typeid(ilang::Object*), "map.create expects type of object");
		ilang::Object *obj = boost::any_cast<ilang::Object*>(args[0]->Get());
		assert(obj);
		error(dynamic_cast<ilang::Array*>(obj) == 0, "map.create does not work on arrays");

		Mapper *map = new Mapper(args[0], obj);

		return ValuePass(new ilang::Value(new ilang::Object(map)));

	}

	ILANG_LIBRARY_NAME("i/map",
										 ILANG_FUNCTION("create", createMapper)
										 )

} // unammed namespace
