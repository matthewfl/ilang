#include "ilang/ilang.h"

#include <curl/curl.h>

namespace {
  using namespace ilang;
  using namespace std;
  static size_t simpleGetCallback (char *data, size_t size, size_t nmemb, void *userp) {
    assert(userp);
    std::string * ret = (std::string*)userp;
    ret->append(data, size * nmemb);
    return size * nmemb;
  }
  ValuePass simpleGet(vector<ValuePass> &args) {
    CURL *curl;
    CURLcode res;
    char bufferError[CURL_ERROR_SIZE];

    std::string ret;

    assert(args.size() == 1);
    assert(args[0]->Get().type() == typeid(std::string));
    
    curl = curl_easy_init();
    assert(curl);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, bufferError);
    curl_easy_setopt(curl, CURLOPT_URL, boost::any_cast<std::string>(args[0]->Get()).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, simpleGetCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&ret);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

    //curl_easy_setopt(curl, CURLOPT_USERAGENT, 
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if(res == CURLE_OK) {
      return ValuePass(new ilang::Value(ret));
    }else{
      std::cerr << "curl error: " << bufferError << endl;
      return ValuePass(new ilang::Value(false));
    }
  }
}


ILANG_LIBRARY_NAME("net/curl",
		   ILANG_FUNCTION("simpleGet", simpleGet);
		   )