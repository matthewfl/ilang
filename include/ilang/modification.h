#ifndef _ilang_modification
#define _ilang_modification

#include "parserTree.h"
#include <string>

namespace ilang {
  struct Mod_Data {
    enum type_t {
      undefined_t,
      string_t
    };
    type_t type;
    std::string string_data;
    union {
      // other pointer stuff that might be returned by the Mod functions
      parserNode::Head *file_head;
      parserNode::Node *node;
      
    };
    Mod_Data(std::string s) : string_data(s), type(string_t) {}
    Mod_Data() : type(undefined_t) {}
  };
  class Modification {
    /* this class will be friends with all the parserTree types so that it can access the internals
     * this class will need to be careful to ensure that it does not break the parserTree beyond repair
     * or cause a memory leak
     */
  private:
    
  public:
    Modification(Mod_Data);
  };
  
} // namespace ilang


#endif // _ilang_modification
