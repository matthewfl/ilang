#include "modification.h"
#include "ilang/ilang.h"

#include <iostream>
using namespace std;

namespace ilang {
  
}

using namespace ilang;

namespace {

  ValuePass FileTree(std::vector<ValuePass> &args) {
    assert(args.size() == 1);
    cout << "mod file called \n";
    if(args[0]->Get().type() == typeid(std::string)) {
      // looking for some file by name
      cout << "mod by file name: " << boost::any_cast<std::string>(args[0]->Get()) << endl;
    }else if(args[0]->Get().type() == typeid(Mod_Data)) {
      std::string dat = boost::any_cast<Mod_Data>(args[0]->Get()).string_data;
      cout << "special passed case: " << dat << endl;
    }
  }



}

ILANG_LIBRARY_NAME("i/mod",
		   //ILANG_CLASS("mod", Mod_class);
		   ILANG_FUNCTION("file", FileTree);
		   import->Set("self", ValuePass(new ilang::Value(Mod_Data("self"))));
		   )
