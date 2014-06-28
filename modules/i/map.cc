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

		ValuePass EmitFunct(Arguments &args) {
			error(args.size() == 2, "map.emit expects 2 arguments");
			error(args[0]->type() == typeid(std::string), "map.emit expects the first argument to be a string");

			m_emitted[args[0]->cast<string>()].push_back(args[1]); // save the emitted element into the map, this will not scale well at the moment.

			return ValuePass(new ilang::Value_Old(true));
		}

		ValuePass MapAction(Arguments &args) {
			error(args.size() == 1, "map.map expects 1 argument");
			error(args[0]->type() == typeid(ilang::Function), "map.map expects a function");
			error(m_obj, "There is no object to map on");

			ilang::Function funct = *args[0]->cast<Function*>();

			//ScopePass obj_scope = ScopePass();
			// if(funct.object) {
			// 	obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(funct.object->Get())));
			// }

			ValuePass ret = ValuePass(new ilang::Value_Old);
			Mapper *returnMapper = new Mapper();
			assert(returnMapper != this); // fcking free bugs
			returnMapper->m_obj = (ilang::Object*)0x2;
			// ilang::Function emitFunct;
			// emitFunct.native = true;
			ilang::Function emit_fun([returnMapper](ScopePass scope, Arguments &args, ValuePass *ret) {
					*ret = returnMapper->EmitFunct(args);
					assert(*ret);
				});
			ValuePass emitFunctVal = ValuePass(new ilang::Value_Old(emit_fun));

			assert(m_obj);
			for(auto vals : m_obj->members) {
				if(vals.first == "this") continue;
				ilang::Variable *var = vals.second;
				ValuePass ret = funct(ValuePass(new ilang::Value_Old(vals.first)), var->Get(), emitFunctVal);
			}

			/*
				#define CALL_WITH(_key, _value)																	\
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
		Mapper() : m_obj(NULL) {
			m_obj = (ilang::Object*)1;
			Init();
		}
				}

				#undef CALL_WITH
			*/

			return ValuePass(new ilang::Value_Old(new ilang::Object(returnMapper)));
		}

		ValuePass Get(Arguments &args) {
			error(args.size() == 1, "map.get expects 1 arugment");
			error(args[0]->type() == typeid(std::string), "map.get expects a string");

			return valueMaker(0);
			// TODO;
			//return ValuePass(new ilang::Value_Old((ilang::Object*) new ilang::Array(m_emitted[
		}

		ValuePass Reduce(Arguments &args) {
			error(args.size() == 1, "map.reduce expects 1 argument");
			error(args[0]->type() == typeid(ilang::Function), "map.reduce expects a function");
			error(!m_obj, "can not reduce directly on an object"); // TODO: remove this constrain

			ilang::Function funct = *args[0]->cast<Function*>();

			// ScopePass obj_scope = ScopePass();
			// if(funct.object) {
			// 	obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(funct.object->Get())));
			// }

			Mapper *returnMapper = new Mapper();
			ilang::Function emitFunct([returnMapper](ScopePass scope, Arguments &args, ValuePass *ret) {
					*ret = returnMapper->EmitFunct(args);
					assert(*ret);
				});
			ValuePass emitFunctVal = ValuePass(new ilang::Value_Old(emitFunct));

#define CALL_WITH(_key, _value)																					\
			{																																	\
				ValuePass ret = funct(ValuePass(new ilang::Value_Old( _key )), _value, emitFunctVal); \
			}

			ValuePass ret = ValuePass(new ilang::Value_Old);

			for(auto vals : m_emitted) {
				ValuePass arr = ValuePass(new ilang::Value_Old((ilang::Object*) new ilang::Array(vals.second)));
				CALL_WITH(vals.first, arr);
			}

#undef CALL_WITH
		}

		void Init() {
			reg("map", &Mapper::MapAction);
			reg("get", &Mapper::Get);
			reg("reduce", &Mapper::Reduce);
		}

		Mapper() : m_obj(NULL) {
			m_obj = (ilang::Object*)1;
			Init();
		}
	public:
		Mapper(ValuePass _hold, ilang::Object *_obj) :
			m_objectHold(_hold), m_obj(_obj)
		{
			assert(m_obj);
			Init();
		}
		~Mapper() {

		}
	};

	ValuePass createMapper(Arguments &args) {
		// when the argument is of a class that has not been constructed, then the type is ilang::Class*, after the obhject is constructed then the type is ilang::Object* with a pointer to class
		error(args.size() == 1, "map.create expects 1 argument");
		error(args[0]->type() == typeid(ilang::Object*), "map.create expects type of object");
		ilang::Object *obj = args[0]->cast<Object*>().get(); //boost::any_cast<ilang::Object*>(args[0]->Get());
		assert(obj);
		error(dynamic_cast<ilang::Array*>(obj) == NULL, "map.create does not work on arrays");

		Mapper *map = new Mapper(args[0], obj);

		return ValuePass(new ilang::Value_Old(new ilang::Object(map)));

	}

	ILANG_LIBRARY_NAME("i/map",
										 ILANG_FUNCTION("create", createMapper)
										 )

} // unammed namespace
