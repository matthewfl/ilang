#ifndef _ilang_modification
#define _ilang_modification

#include "parserTree.h"
#include <string>

namespace ilang {
  struct ModData;
  class Modification {
    /* this class will be friends with all the parserTree types so that it can access the internals
     * this class will need to be careful to ensure that it does not break the parserTree beyond repair
     * or cause a memory leak
     */
  public:
    enum types {
      unknown_t = 0,
      // major types
      file_t = 1,
      value_t,
      stmt_t,
      variable_t,
      expression_t,
      constant_t = 10,
      function_t, // this is also viewed as a value, but there are so many import things involving it
      // secondary times
      object_t = 20,
      class_t,
      array_t,
      stringConst_t = 30,
      intConst_t,
      floatConst_t,
      ifStmt_t = 40,
      whileStmt_t,
      forStmt_t,
      returnStmt_t,
      normalVariable_t = 50,
      fieldAccess_t,
      arrayAccess_t,
      call_t = 60,
      newCall_t,
      printCall_t,
      assignExpr_t = 70,
      mathExpr_t,
      logicExpr_t
    };
  private:
    types m_masterType;
    types m_secondaryType;
    union {
      ilang::parserNode::Head* file;
      ilang::parserNode::Node* node;
    };

    // private functions
    void FigureType(ilang::parserNode::Node*);
    void Init();
  public:
    Modification(ModData);
    Modification(ilang::parserNode::Node*);
    Modification(ilang::parserNode::Head*);

    // general helper functions that are used throughout
    static FileScope* getFileScope(Scope*);
  };
  

  struct ModData {
    enum type_t {
      undefined_t,
      string_t,
      map_type_t
    };
    type_t type;
    std::string string_data;
    union {
      ilang::Modification::types map_type;
    };
    ModData(std::string s) : string_data(s), type(string_t) {}
    ModData(Modification::types t) : map_type(t), type(map_type_t) {}
    ModData() : type(undefined_t) {}
  };
} // namespace ilang


#endif // _ilang_modification
