#include "ilang/ilang.h"
#include "error.h"

#include "../../deps/libuv/include/uv.h"
#include "../../deps/http-parser/http_parser.h"

extern "C" {
#include "../../deps/http-parser/http_parser.c"
}

#define SERVER_QUEUE_BUFFER_SIZE 128


namespace {
  using namespace std;
  using namespace ilang;

  uv_buf_t on_alloc(uv_handle_t *client, size_t suggested_size) {
    uv_buf_t ret;
    ret.base = new char[suggested_size];
    ret.len = suggested_size;
    return ret;
  }

  class Request;

  class Server : public C_Class {
  private:
    friend class Request;
    ValuePass callback;
    bool listening;
    int listenPort;

    uv_loop_t *loop;
    uv_tcp_t server;
    http_parser_settings parser_settings;

    void setUpUV(int port);
    static void on_connection_cb(uv_stream_t *server, int status);
    static void on_read_cb(uv_stream_t *stream, ssize_t nread, uv_buf_t buf);

    ValuePass isRunning(Scope *scope, vector<ValuePass> &args);
    ValuePass setListen(Scope *scope, vector<ValuePass> &args);
    ValuePass startListen(Scope *scope, vector<ValuePass> &args);
    ValuePass stopListen(Scope *scope, vector<ValuePass> &args);
    ValuePass waitEnd(Scope *scope, vector<ValuePass> &args);
    void Init();

  public:
    Server(ValuePass call, int port);
    virtual ~Server();

  };

  class Request : public C_Class {
  private:
    friend class Server;
    Server *parent;

    uv_tcp_t handle;
    http_parser parser;
    uv_write_t write_req;



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

    ValuePass writeHead(vector<ValuePass> &args) {
      // eg: writeHead(302, "Location: http://......", "Content-type: text/plain", .....);
      int code = 200;
      if(args.size() > 1) {
	error(args[0]->Get().type() == typeid(long), "httpd.request.writeHead expects a responce code for the first argument");
	code = boost::any_cast<long>(args[0]->Get());
	error(code < 1000 && code > 99, "Http code expects to be between 100 and 999");
      }
      for(int i=0;i<args.size();i++) {
	error(args[i]->Get().type() == typeid(std::string), "httpd.request.writeHead expects a string for the 2+ arguments");
      }
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
      reg("headers", &Request::getHeader);
      //reg("type", &Request::reqType);
      //reg("getPost", &Request::getPost);

      reg("writeHead", &Request::writeHead);
      reg("write", &Request::writeReq);
      reg("end", &Request::endReq);

      uv_tcp_init(parent->loop, &handle);
      http_parser_init(&parser, HTTP_REQUEST);
      handle.data = this;
      parser.data = this;


    }
  public:
    Request(Server *p /* some data from the request */) {
      parent = p;

      Init();
    }
  };

  // Server start


  void Server::setUpUV(int port) {
    assert(loop);
    assert(port >= 0 && port <= 0xFFFF);
    int r;
    r = uv_tcp_init(loop, &server);
    assert(!r);
    struct sockaddr_in address = uv_ip4_addr("0.0.0.0", port);
    r = uv_tcp_bind(&server, address);
    assert(!r);
    server.data = (void*)this;
  }

  void Server::on_connection_cb (uv_stream_t *server, int status) {
    Server *ser = (Server*)server->data;
    assert(ser);
    assert((uv_tcp_t*)server == &ser->server);
    //ser->on_connection(server, status);
    Request *req = new Request(ser);
    int r = uv_accept(server, (uv_stream_t*)&req->handle);
    assert(!r);
    uv_read_start((uv_stream_t*)&req->handle, on_alloc, on_read_cb);
  }

  void Server::on_read_cb(uv_stream_t *stream, ssize_t nread, uv_buf_t buf) {

  }

  /*void on_connection (uv_stream_t *server, int status) {

    }*/

  ValuePass Server::isRunning(Scope *scope, vector<ValuePass> &args) {
    error(args.size() == 0, "httpd.running does not take any arguments");
    return ValuePass(new ilang::Value(listening));
  }

  ValuePass Server::setListen(Scope *scope, vector<ValuePass> &args) {
    error(args.size() == 1, "http.setListen takes 1 argument");
    error(args[0]->Get().type() == typeid(ilang::Function), "http.setListen takes a function for an argument");

    callback = args[0];

    return ValuePass(new ilang::Value);
  }

  ValuePass Server::startListen(Scope *scope, vector<ValuePass> &args) {
    error(args.size() <= 1, "httpd.start takes one argument");
    error(listening == false, "httpd already listening");

    if(args.size()) {
      error(args[0]->Get().type() == typeid(long), "argument to httpd.start expected to be a number");
      listenPort = boost::any_cast<long>(args[0]->Get());
    }

    // fill in
    setUpUV(listenPort);
    uv_listen((uv_stream_t*)&server,
	      SERVER_QUEUE_BUFFER_SIZE,
	      on_connection_cb);
    uv_run(loop);

    return ValuePass(new ilang::Value);

  }
  ValuePass Server::stopListen(Scope *scope, vector<ValuePass> &args) {
    error(args.size()== 0, "httpd.stop takes no arguments");
    error(listening == true, "httpd not listening");

    // fill in
    // close the socket on loop

    return ValuePass(new ilang::Value);
  }

  ValuePass Server::waitEnd(Scope *scope, vector<ValuePass> &args) {

  }

  void Server::Init() {
    reg("running", &Server::isRunning);
    reg("listen", &Server::startListen);
    reg("stop", &Server::stopListen);
    reg("setCallback", &Server::setListen);
    reg("wait", &Server::waitEnd);

    loop = uv_loop_new();
  }

  Server::Server(ValuePass call, int port) {
    listening = false;
    callback = call;
    listenPort = port;
    Init();
  }

  Server::~Server() {
    // running the loop when the server is deleted might be an issue
    // uv_run(loop);
    uv_loop_delete(loop);
  }

  // Server end


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
