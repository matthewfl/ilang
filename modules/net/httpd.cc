#include "ilang/ilang.h"
#include "error.h"

#include "../../deps/libuv/include/uv.h"


namespace {
  using namespace std;
  using namespace ilang;

  class Request : public C_Class {
  private:

    ValuePass getUrl (vector<ValuePass> &args) {
      error(args.size() == 0, "httpd.request.url expects no arguments");

      return ValuePass(new ilang::Value);
    }

    ValuePass getHeader (vector<ValuePass> &args) {
      error(args.size() == 1, "httpd.request.header expects 1 argument");
      error(args[0]->Get().type() == typeid(std::string), "httpd.request.header expects a string type");

      // read data from the header and return it back
      return ValuePass(new ilang::Value(std::string("")));
    }

    ValuePass writeReq(vector<ValuePass> &args) {
      error(args.size() > 1, "httpd.request.write expects at least one argument");
      for(ValuePass val : args) {
	// create buffers for all the iterms
	// send to libuv
      }
    }

    ValuePass endReq(vector<ValuePass> &args) {
      if(args.size() != 0) {
	ValuePass ii = writeReq(args);
      }
    }

    void Init() {
      reg("url", &Request::getUrl);
      reg("header", &Request::getHeader);
      reg("end", &Request::endReq);
    }
  public:
    Request(/* some data from the request */) {

      Init();
    }
  };

  class Server : public C_Class {
  private:
    ValuePass callback;
    bool listening;
    uv_loop_t *loop;


    ValuePass isRunning(Scope *scope, vector<ValuePass> &args) {
      error(args.size() == 0, "httpd.running does not take any arguments");
      return ValuePass(new ilang::Value(listening));
    }

    ValuePass setListen(Scope *scope, vector<ValuePass> &args) {
      error(args.size() == 1, "http.setListen takes 1 argument");
      error(args[0]->Get().type() == typeid(ilang::Function), "http.setListen takes a function for an argument");

      return ValuePass(new ilang::Value);
    }

    ValuePass startListen(Scope *scope, vector<ValuePass> &args) {
      error(args.size() == 0, "httpd.start takes no arguments");
      error(listening == false, "httpd already listening");

      // fill in

      return ValuePass(new ilang::Value);

    }
    ValuePass stopListen(Scope *scope, vector<ValuePass> &args) {
      error(args.size()== 0, "httpd.stop takes no arguments");
      error(listening == true, "httpd not listening");

      // fill in

      return ValuePass(new ilang::Value);
    }

    ValuePass waitEnd(Scope *scope, vector<ValuePass> &args) {

    }

    void Init() {
      reg("running", &Server::isRunning);
      reg("start", &Server::startListen);
      reg("stop", &Server::stopListen);
      reg("listen", &Server::setListen);
      reg("wait", &Server::waitEnd);

      loop = uv_loop_new();
    }
  public:
    Server(ValuePass call, int port) {
      listening = false;
      callback = call;
      Init();
    }

    virtual ~Server() {
      // running the loop when the server is deleted might be an issue
      // uv_run(loop);
      uv_loop_delete(loop);
    }
  };


  ValuePass createServer(vector<ValuePass> &args) {
    error(args.size() == 2, "httpd.create takes 2 arguments");
    error(args[0]->Get().type() == typeid(long), "httpd.create 1st argument is a port number");
    error(args[1]->Get().type() == typeid(ilang::Function), "httpd.create 2nd argument is a callback function");
    Server *ser = new Server(args[1], boost::any_cast<long>(args[0]->Get()));
    return ValuePass(new ilang::Value(new ilang::Object(ser)));
  }
}

ILANG_LIBRARY_NAME("net/httpd",
		   ILANG_FUNCTION("create", createServer)
		   )
