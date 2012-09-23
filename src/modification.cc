#include "modification.h"
#include "ilang/ilang.h"
#include "parserTree.h"
#include "error.h"
#include "debug.h"

#include <iostream>
using namespace std;

namespace ilang {
  void Modification::FigureType (ilang::parserNode::Node *node) {
    assert(node);
    using namespace ilang::parserNode;
    if(dynamic_cast<ilang::parserNode::Value*>(node)) {
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
    m_masterType = m_secondaryType = unknown_t;
  }


  bool Modification::isType(types t) {
    return m_masterType == t || m_secondaryType == t;
  }

  Modification::Modification(ModData dat) {

  }



  // static functions under modification
  // modification is also viewed as a supper class that is allowed to touch everything
  FileScope *Modification::getFileScope(Scope* s) {
    while(s->parent) s = s->parent;
    return dynamic_cast<FileScope*>(s);
  }

}

using namespace ilang;

namespace {
  class Modification_manager : public ilang::C_Class {
  private:
    // private variables
    Modification *mod;
  public:
    ilang::ValuePass isType(Scope *scope, std::vector<ilang::ValuePass> &args) {
      cout << "class type call\n";
      error(args.size() == 1, "mod.manager.type expectes 1 argument");
      error(args[0]->Get().type() == typeid(ModData), "mod.manager.type expectes given type to be of mod.type.*");
      ModData d = boost::any_cast<ModData>(args[0]->Get());
      error(d.type == ModData::map_type_t, "mod.manager.type expectes given type to be of mod.type.*");
      if(!mod) return ValuePass(new ilang::Value(bool( d.map_type == Modification::unknown_t )));
      return ValuePass(new ilang::Value(bool( mod->isType(d.map_type) )));
    }
  private:
    void Init () {
      reg("Type", &Modification_manager::isType);
    }
  public:
    Modification_manager (): mod(NULL) {
      Init();
    }
    Modification_manager(Modification *m) : mod(m) {
      assert(m);

    }
  };


  ValuePass FileTree(Scope *scope, std::vector<ValuePass> &args) {
    assert(args.size() == 1);
    cout << "mod file called \n";
    if(args[0]->Get().type() == typeid(std::string)) {
      // looking for some file by name
      cout << "mod by file name: " << boost::any_cast<std::string>(args[0]->Get()) << endl;
    }else if(args[0]->Get().type() == typeid(ModData)) {
      ModData dat = boost::any_cast<ModData>(args[0]->Get());
      cout << "special passed case: " << dat.string_data << endl;
      if(dat.string_data == "self_file") {
	cout << "getting self\n" << flush;
	Modification::getFileScope(scope);
	//Modification_manager m;
	ilang::Value * val = new ilang::Value(new Object(new Modification_manager));
	return ValuePass(val);
	//Scope *looking = scope;
	//while(looking->parent) looking = looking->parent;
	// try and just get the
      }
    }else assert(0);
  }



}

ILANG_LIBRARY_NAME("i/mod",
		   //ILANG_CLASS("mod", Mod_class);
		   ILANG_FUNCTION("file", FileTree);
		   import->Set("self", ValuePass(new ilang::Value(ModData("self_file"))));
		   Object * type_obj = new Object;
		   type_obj->operator[]("Unknown")->Set(ValuePass(new ilang::Value(ModData(Modification::unknown_t))));
		   type_obj->operator[]("Function")->Set(ValuePass(new ilang::Value(ModData(Modification::function_t))));
		   type_obj->operator[]("Object")->Set(ValuePass(new ilang::Value(ModData(Modification::object_t))));
		   type_obj->operator[]("Class")->Set(ValuePass(new ilang::Value(ModData(Modification::class_t))));
		   type_obj->operator[]("Variable")->Set(ValuePass(new ilang::Value(ModData(Modification::variable_t))));
		   //type_obj->operator[]("function")->Set(ValuePass(new ilang::Value(ModData(Modification::function_t))));
		   //type_obj->operator[]("function")->Set(ValuePass(new ilang::Value(ModData(Modification::function_t))));
		   //type_obj->operator[]("function")->Set(ValuePass(new ilang::Value(ModData(Modification::function_t))));

		   import->Set("type", ValuePass(new ilang::Value(type_obj)));
		   )
