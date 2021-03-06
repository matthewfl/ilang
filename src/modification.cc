#include "modification.h"
#include "ilang.h"
#include "parserTree.h"
#include "error.h"
#include "debug.h"
#include "print.h"

#include <iostream>
#include <sstream>
using namespace std;

namespace ilang {
	void Modification::FigureType (ilang::parserNode::Node *node) {
		errorTrace("Modification::FigureType");
		assert(node);
		using namespace ilang::parserNode;
		if(dynamic_cast<ilang::parserNode::Value_Old*>(node)) {
			if(dynamic_cast<Constant*>(node)) {
				m_masterType = constant_t;
				if(dynamic_cast<StringConst*>(node)) {
					m_secondaryType = stringConst_t;
				}else if(dynamic_cast<IntConst*>(node)) {
					m_secondaryType = intConst_t;
				}else if(dynamic_cast<FloatConst*>(node)) {
					m_secondaryType = floatConst_t;
				}
			}else if(dynamic_cast<Expression*>(node)) {
				m_masterType = expression_t;
				if(dynamic_cast<AssignExpr*>(node)) {
					m_secondaryType = assignExpr_t;
				}else if(dynamic_cast<MathEquation*>(node)) {
					m_secondaryType = mathExpr_t;
				}else if(dynamic_cast<LogicExpression*>(node)) {
					m_secondaryType = logicExpr_t;
				}else if(dynamic_cast<SingleExpression*>(node)) {
					m_secondaryType = singleExpr_t;
				}
			}else if(dynamic_cast<ilang::parserNode::Object*>(node)) {
				m_masterType = m_secondaryType = object_t; // TODO: change the master and secondary based off what is useful in working with them
			}else if(dynamic_cast<ilang::parserNode::Class*>(node)) {
				m_masterType = m_secondaryType = class_t;
			}else if(dynamic_cast<ilang::parserNode::Array*>(node)) {
				m_masterType = m_secondaryType = array_t;
			}else if(dynamic_cast<ilang::parserNode::Function*>(node)) {
				m_masterType = m_secondaryType = function_t;
			}else if(dynamic_cast<ilang::parserNode::Variable*>(node)) {
				m_masterType = variable_t;
				if(dynamic_cast<FieldAccess*>(node)) {
					m_secondaryType = fieldAccess_t;
				}else if(dynamic_cast<ArrayAccess*>(node)) {
					m_secondaryType = arrayAccess_t;
				}else{
					m_secondaryType = variable_t;
				}
			}else if(dynamic_cast<Call*>(node)) {
				m_masterType = call_t;
				if(dynamic_cast<PrintCall*>(node)) {
					m_secondaryType = printCall_t;
				}else if(dynamic_cast<NewCall*>(node)) {
					m_secondaryType = newCall_t;
				}else if(dynamic_cast<ImportCall*>(node)) {
					m_secondaryType = importCall_t;
				}else{
					m_secondaryType = call_t;
				}
			}
		}else if(dynamic_cast<IfStmt*>(node)) {
			m_masterType = stmt_t;
			m_secondaryType = ifStmt_t;
		}else if(dynamic_cast<WhileStmt*>(node)) {
			m_masterType = stmt_t;
			m_secondaryType = whileStmt_t;
		}else if(dynamic_cast<ForStmt*>(node)) {
			m_masterType = stmt_t;
			m_secondaryType = forStmt_t;
		}else if(dynamic_cast<ReturnStmt*>(node)) {
			m_masterType = stmt_t;
			m_secondaryType = returnStmt_t;
		}else{
			// um....
			assert(0);
		}
	}

	void Modification::Init () {
		m_file = NULL;
		m_node = NULL;
		m_value = NULL;
		m_masterType = m_secondaryType = unknown_t;
	}


	/*Modification::Modification(ModData dat) {

		}*/

	Modification::Modification(FileScope *scope) {
		Init();
		m_file = scope->head;
		m_masterType = m_secondaryType = file_t;
	}
	Modification::Modification(ilang::parserNode::Head *head) {
		Init();
		m_file = head;
		m_masterType = m_secondaryType = file_t;
	}
	Modification::Modification(ilang::parserNode::Node *node) {
		Init();
		m_node = node;
		FigureType(node);
	}
	Modification::Modification(ilang::ValuePass val) {
		Init();
		m_valuePassHold = val;
		cout << "Creating new modification with " << val->Get().type().name() << endl;
		if(val->Get().type() == typeid(ilang::Function)) {
			Function fun =	boost::any_cast<ilang::Function>(val->Get());
			// TODO: omg
			//			if(fun.native) {
				error(0, "Can not perform modification of native function");
				//}
				//m_node = dynamic_cast<parserNode::Node*>(fun.func);
				//FigureType(m_node);
		}else if(val->Get().type() == typeid(ilang::Class*)) {
			ilang::Class *cla = boost::any_cast<ilang::Class*>(val->Get());
			//assert(0); // not written yet
		}else if(val->Get().type() == typeid(ilang::Object*)) {
			ilang::Object *obj = boost::any_cast<ilang::Object*>(val->Get());
			//assert(0); // not written yet
		}else{
			error(0, "parserNode not found from type");
		}
	}
	Modification::Modification(ilang::Variable *var) :Modification(var->Get()) {
		m_name = var->Name;
	}

	vector<ilang::Modification*> Modification::getList() {
		errorTrace("Modification::getList");
		vector<ilang::Modification*> ret;
		if(isType(file_t)) {
			// return everything that is assigned in the global scope
			/*if(m_file->scope) {
			// we might as well return from the global scope as this will hold more revelant data
			for(auto it : m_file->scope->vars) {
			Modification *mod = new Modification(it.second);
			mod->m_name = it.first;
			ret.push_back(mod);
			}
			}else*/
			if(m_file->Declars) {
				// if the declares are checked, then the system would be able to more easily modify the contents of the file however then changing what is currently set would be harder
				for(auto it : *m_file->Declars) {
					Modification *mod = new Modification(it);
					ret.push_back(mod);
				}
			}else{
				assert(0); // there should be no way that this can ever happen
			}
		}else if(isType(function_t)) {
			// return all the statements in the function
			// need to deal with the difference in working with it in the form of a value
			// and
			parserNode::Function *func = dynamic_cast<parserNode::Function*>(m_node);
			assert(func);
			for(auto it : *(func->body)) {
				Modification *mod = new Modification(it);
				ret.push_back(mod);
			}
		}else if(isType(class_t)) {
			// return everything that is assigned in the class
			parserNode::Class *cla = dynamic_cast<parserNode::Class*>(m_node);
			assert(cla);
			for(auto it : *cla->objects) {
				Modification *mod = new Modification(it.second);
				mod->m_name = it.first->GetFirstName();
				ret.push_back(mod);
			}
		}else if(isType(object_t)) {
			// return everything that is assigned in the object
			assert(0);
		}

		return ret;
	}

	string Modification::print() {
		errorTrace("Modification::print");
		stringstream str;
		Printer pp(&str);
		if(isType(unknown_t)) {
			return "-UNKNOWN TYPE-";
		}else if(isType(file_t)) {
			assert(m_file);
			m_file->Print(&pp);
		}else{
			assert(m_node);
			m_node->Print(&pp);
		}
		return str.str();
	}


	// static functions under modification
	// modification is also viewed as a supper class that is allowed to touch everything
	FileScope *Modification::getFileScope(ScopePass s) {
		while(s->parent) s = s->parent;
		return dynamic_cast<FileScope*>(s.get());
	}

}

using namespace ilang;

namespace {
	class Modification_manager : public ilang::C_Class {
	private:
		// private variables
		Modification *mod;
		ilang::ValuePass isType(Scope *scope, std::vector<ilang::ValuePass> &args) {
			//cout << "class type call\n";
			error(args.size() == 1, "mod.manager.type expectes 1 argument");
			error(args[0]->Get().type() == typeid(ModData), "mod.manager.type expectes given type to be of mod.type.*");
			ModData d = boost::any_cast<ModData>(args[0]->Get());
			error(d.type == ModData::map_type_t, "mod.manager.type expectes given type to be of mod.type.*");
			if(!mod) return ValuePass(new ilang::Value_Old(bool( d.map_type == Modification::unknown_t )));
			return ValuePass(new ilang::Value_Old(bool( mod->isType(d.map_type) )));
		}
		ilang::ValuePass scopeList(Scope *scope, std::vector<ilang::ValuePass> &args) {
			error(args.size() == 0, "mod.manager.list does not take any arguments");
			vector<ValuePass> ret;
			error(mod, "mod.manager, no Modification set");
			vector<Modification*> mods = mod->getList();
			ret.reserve(mods.size());
			for(auto it : mods) {
				Modification *m = it;

				ret.push_back(ValuePass(new ilang::Value_Old(new Object(new Modification_manager(m)))));
			}
			//mod->get

			ilang::Object *ret_arr = new ilang::Array(ret);
			return ValuePass(new ilang::Value_Old(ret_arr));
		}

		ilang::ValuePass map(Scope *scope, std::vector<ilang::ValuePass> &args) {
			error(args.size() == 1, "mod.manager.each takes one argument");
			error(args[0]->Get().type() == typeid(ilang::Function), "mod.manager.each takes a function for an argument");

			ilang::Function *func = boost::any_cast<ilang::Function>(& args[0]->Get());

			vector<Modification*> list = mod->getList();
			ValuePass ret = ValuePass(new ilang::Value_Old);

			/*if(func->object) {
				ValuePass ret = ValuePass(new ilang::Value_Old);
				assert(func->object->Get().type() == typeid(ilang::Object*));
				ScopePass obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(func->object->Get())));
				for(Modification *it : list) {
					vector<ValuePass> param;
					ilang::Value_Old * val = new ilang::Value_Old(new Object(new Modification_manager(it)));
					param.push_back(ValuePass(val));
					func->ptr(obj_scope, param, &ret);
					if(ret->isTrue()) break;
					}*/
			}else if(func->native) {
				// why would we have a native function here
				error(0, "Modification.each with a native function, not done stuff");
			}else{
				ValuePass ret = ValuePass(new ilang::Value_Old);
				for(Modification *it : list) {
					vector<ValuePass> param;
					ilang::Value_Old * val = new ilang::Value_Old(new Object(new Modification_manager(it)));
					param.push_back(ValuePass(val));
					func->ptr(ScopePass(), param, &ret);
					if(ret->isTrue()) break;
				}
			}
			return ValuePass(new ilang::Value_Old);
		}

		ilang::ValuePass print(Scope *scope, std::vector<ilang::ValuePass> &args) {
			error(args.size() == 0, "print does not take any arguments");
			return ValuePass(new ilang::Value_Old(std::string(mod->print())));
		}

		ilang::ValuePass getName(Scope *scope, std::vector<ilang::ValuePass> &args) {
			error(args.size() == 0, "getName does not take arguments");
			return ValuePass(new ilang::Value_Old(mod->getName()));
		}
	private:
		void Init () {
			reg("type", &Modification_manager::isType);
			reg("is", &Modification_manager::isType);
			reg("list", &Modification_manager::scopeList);
			reg("map", &Modification_manager::map);
			reg("str", &Modification_manager::print);
			reg("name", &Modification_manager::getName);
		}
	public:
		Modification_manager (): mod(NULL) {
			Init();
		}
		Modification_manager(Modification *m) : mod(m) {
			assert(m);
			Init();
		}
	};


	ValuePass FileTree(ScopePass scope, std::vector<ValuePass> &args) {
		errorTrace("mod.file");
		error(args.size() == 1, "mod.file expects 1 argument");
		//cout << "mod file called \n";
		if(args[0]->Get().type() == typeid(std::string)) {
			// looking for some file by name
			cout << "mod by file name: " << boost::any_cast<std::string>(args[0]->Get()) << endl;
		}else if(args[0]->Get().type() == typeid(ModData)) {
			ModData dat = boost::any_cast<ModData>(args[0]->Get());
			cout << "special passed case: " << dat.string_data << endl;
			if(dat.string_data == "self_file") {
				cout << "getting self\n" << flush;
				FileScope *s = Modification::getFileScope(scope);
				Modification *m = new Modification(s);
				//Modification_manager m;
				ilang::Value_Old * val = new ilang::Value_Old(new Object(new Modification_manager(m)));
				return ValuePass(val);
				//Scope *looking = scope;
				//while(looking->parent) looking = looking->parent;
				// try and just get the
			}
		}else{
			assert(0);
		}
	}


	ValuePass createNode(Scope *scope, std::vector<ValuePass> &args) {
		errorTrace("mod createNode");
		error(args.size() == 1, "mod.node expects 1 argument");
		Modification *m = new Modification(args[0]);
		ilang::Value_Old * val = new ilang::Value_Old(new Object(new Modification_manager(m)));
		return ValuePass(val);

	}


}

ILANG_LIBRARY_NAME("i/mod",
									 //ILANG_CLASS("mod", Mod_class);
									 ILANG_FUNCTION("file", FileTree);
									 ILANG_FUNCTION("node", createNode);
									 ILANG_FUNCTION("of", createNode);
									 import->Set("self", ValuePass(new ilang::Value_Old(ModData("self_file"))));
									 import->Set("stop", ValuePass(new ilang::Value_Old(ModData("stop"))));
									 Object * type_obj = new Object;
									 type_obj->operator[]("Unknown")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::unknown_t))));
									 type_obj->operator[]("File")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::file_t))));
									 type_obj->operator[]("Value")->Set(ValuePass(new ilang::Value(ModData(Modification::value_t))));
									 type_obj->operator[]("Expression")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::unknown_t))));
									 type_obj->operator[]("Constant")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::constant_t))));

									 type_obj->operator[]("Function")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::function_t))));
									 type_obj->operator[]("Object")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::object_t))));
									 type_obj->operator[]("Class")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::class_t))));
									 type_obj->operator[]("Array")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::array_t))));
									 type_obj->operator[]("StringConst")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::stringConst_t))));
									 type_obj->operator[]("IntConst")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::intConst_t))));
									 type_obj->operator[]("FloatConst")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::floatConst_t))));

									 type_obj->operator[]("If")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::ifStmt_t))));
									 type_obj->operator[]("For")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::forStmt_t))));
									 type_obj->operator[]("While")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::whileStmt_t))));
									 type_obj->operator[]("Return")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::returnStmt_t))));

									 type_obj->operator[]("Variable")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::variable_t))));
									 type_obj->operator[]("FieldAccess")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::fieldAccess_t))));
									 type_obj->operator[]("ArrayAccess")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::arrayAccess_t))));

									 type_obj->operator[]("Call")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::call_t))));
									 type_obj->operator[]("NewCall")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::newCall_t))));
									 type_obj->operator[]("PrintCall")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::printCall_t))));

									 type_obj->operator[]("AssignExpression")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::assignExpr_t))));
									 type_obj->operator[]("MathExpression")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::mathExpr_t))));
									 type_obj->operator[]("LogicExpression")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::logicExpr_t))));
									 type_obj->operator[]("SingleExpression")->Set(ValuePass(new ilang::Value_Old(ModData(Modification::singleExpr_t))));
									 //type_obj->operator[]("function")->Set(ValuePass(new ilang::Value(ModData(Modification::function_t))));
									 //type_obj->operator[]("function")->Set(ValuePass(new ilang::Value(ModData(Modification::function_t))));
									 //type_obj->operator[]("function")->Set(ValuePass(new ilang::Value(ModData(Modification::function_t))));

									 import->Set("type", ValuePass(new ilang::Value_Old(type_obj)));
									 )
