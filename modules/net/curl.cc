#include "ilang/ilang.h"

#include <curl/curl.h>

namespace {
  using namespace ilang;
  using namespace std;
  ValuePass simpleGet(vector<ValuePass> &args) {
    CURL *curl;
    CURLcode res;
    
    assert(args.size() == 1);
    assert(args[0]->Get().type() == typeid(std::string));
    
    curl = curl_easy_init();
    assert(curl);
    curl_easy_setopt(curl, CURLOPT_URL, "");
    
    return ValuePass(new ilang::Value);
  }
}


ILANG_LIBRARY_NAME("net/curl",
		   ILANG_FUNCTION("simpleGet", simpleGet)
		   )
