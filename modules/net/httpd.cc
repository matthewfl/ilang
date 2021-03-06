#include "ilang.h"
#include "error.h"
#include "function.h"
#include "object.h"
#include "debug.h"

//#include <boost/thread/shared_mutex.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <string>


#include "../../deps/libuv/include/uv.h"
#include "../../deps/http-parser/http_parser.h"

extern "C" {
#include "../../deps/http-parser/http_parser.c"
}

#define SERVER_QUEUE_BUFFER_SIZE 128

#include <iostream>
using namespace std;

/**
 * Need to make the Request::close non blocking
 * best way would be to prob make Request into some type of wrapper class
 * and then when Request is destroyed it can tell the wrapped class to closed
 * and destroy itself.
 */

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
		ValuePass _callback;
		Function callback;
		bool listening;
		int listenPort;

		uv_loop_t *loop;
		uv_tcp_t server;
		http_parser_settings parser_settings = {0,0,0,0,0,0,0};

		void setUpUV(int port);
		static void on_connection_cb(uv_stream_t *server, int status);
		static void on_read_cb(uv_stream_t *stream, ssize_t nread, uv_buf_t buf);
		static void on_close(uv_handle_t *handle);
		static int header_complete_cb(http_parser *parser);
		static int url_cb(http_parser *parser, const char *dat, size_t length);
		static int header_field_cb(http_parser *parser, const char *dat, size_t length);
		static int header_value_cb(http_parser *parser, const char *dat, size_t length);


		ValuePass isRunning(Context &ctx, Arguments &args);
		ValuePass setListen(Context &ctx, Arguments &args);
		ValuePass startListen(Context &ctx, Arguments &args);
		ValuePass stopListen(Context &ctx, Arguments &args);
		ValuePass waitEnd(Context &ctx, Arguments &args);
		void Init();

	public:
		Server(ValuePass call, int port);
		virtual ~Server();

	};

	class Request : public C_Class {
	private:
		friend class Server;
		Server *parent;
		bool headWritten = false;
		bool reqOpen = true;
		string url;
		string last_header;
		map<string, string> headers;

		//boost::shared_mutex close_mutex;

		uv_tcp_t *handle;
		http_parser parser;
		uv_write_t write_req;



		ValuePass getUrl (Context &ctx, Arguments &args) {
			error(args.size() == 0, "httpd.request.url expects no arguments");
			return valueMaker(url);
		}

		ValuePass getHeader (Context &ctx, Arguments &args) {
			error(args.size() == 1, "httpd.request.header expects 1 argument");
			error(args[0]->type() == typeid(std::string), "httpd.request.header expects a string type");
			string s = args[0]->cast<std::string>();
			boost::algorithm::to_lower(s);

			// read data from the header and return it back
			return valueMaker(headers[s]);
		}

		ValuePass writeHead(Context &ctx, Arguments &args) {
			debug(4, "in write head\n");
			// eg: writeHead(302, "Location: http://......", "Content-type: text/plain", .....);
			//close_mutex.lock_shared();
			int code = 200;
			if(args.size() >= 1) {
				error(args[0]->type() == typeid(long), "httpd.request.writeHead expects a responce code for the first argument");
				code = args[0]->cast<int>();
				error(code < 1000 && code > 99, "Http code expects to be between 100 and 999");
			}
			for(int i=1;i<args.size();i++) {
				error(args[i]->type() == typeid(std::string), "httpd.request.writeHead expects a string for the 2+ arguments");
			}

			struct headWriteReq {
				Request *parent;
				uv_write_t req;
				uv_buf_t *buf;
				size_t buf_size;
			};

			assert(!headWritten);
			headWritten = true;

			headWriteReq *writereq = new headWriteReq;
			writereq->parent = this;
			writereq->req.data = writereq;

			int length = args.size();
			if(length == 0) length = 1;
			length++;
			uv_buf_t *buf = new uv_buf_t[length];
			writereq->buf = buf;
			writereq->buf_size = length;



			string status_code = "HTTP/1.1 ";
			status_code += to_string(code);
			status_code += " OK \r\n";
			buf[0].len = status_code.size();
			buf[0].base = new char[status_code.size()];
			status_code.copy(buf[0].base, status_code.length());
			int place=1;
			for(;place<args.size();place++) {
				string str = args[place]->cast<string>();
				str += "\r\n";
				buf[place].len = str.length();
				buf[place].base = new char[str.length()];
				str.copy(buf[place].base, str.length());
			}
			buf[place].base = new char[2];
			buf[place].len = 2;
			buf[place].base[0] = '\r';
			buf[place].base[1] = '\n';

			uv_write(&writereq->req, (uv_stream_t*)handle, buf, length,
							 [](uv_write_t* req, int status) -> void {
								 debug(4, "-----------------closing head write\n" << flush );
								 headWriteReq *writereq = (headWriteReq*) req->data;
								 assert(req == &writereq->req);
								 //writereq->parent->close_mutex.unlock_shared();
								 for(int i=0;i<writereq->buf_size;i++) {
									 delete[] writereq->buf[i].base;
								 }
								 delete[] writereq->buf;
								 delete writereq;
							 });

			debug(4, "end of write head\n") ;

			return ValuePass();
		}

		ValuePass writeReq(Context &ctx, Arguments &args) {
			debug(4, "in write req\n")
				//close_mutex.lock_shared();
				error(args.size() >= 1, "httpd.request.write expects at least one argument");
			if(headWritten == false) {
				Arguments headArgs;
				ValuePass ii = writeHead(ctx, headArgs);
			}

			struct writeReq {
				Request *parent;
				uv_write_t req;
				uv_buf_t *buf;
				size_t buf_size;
			};

			writeReq *writereq = new writeReq;
			writereq->req.data = writereq;

			int place=0;
			uv_buf_t *buf = new uv_buf_t[args.size()];
			writereq->buf = buf;
			writereq->buf_size = args.size();
			for(auto val : args) {
				// create buffers for all the iterms
				// send to libuv
				string str = val.second->cast<string>();
				debug(4, "writing req with "<< str );
				buf[place].len = str.length();
				buf[place].base = new char[str.length()];
				str.copy(buf[place].base, str.length());
				place++;
			}
			//buf[place].len=0;
			//buf[place].base = 0;
			uv_write(&writereq->req, (uv_stream_t*)handle, buf, args.size(),
							 [](uv_write_t *req, int status) -> void {
								 debug(4, "closing write req" << flush );
								 writeReq *writereq = (writeReq*) req->data;
								 //writereq->parent->close_mutex.unlock_shared();
								 for(int i=0;i < writereq->buf_size; i++) {
									 delete[] writereq->buf[i].base;
								 }
								 delete[] writereq->buf;
								 delete writereq;
							 });

			debug(4, "end of write req");
			return valueMaker(true);
		}

		ValuePass endReq(Context &ctx, Arguments &args) {
			if(args.size() != 0) {
				ValuePass ii = writeReq(ctx, args);
			}
			close();
			return ValuePass();
		}

		void Init() {
			handle = new uv_tcp_t;
			reg("url", &Request::getUrl);
			reg("headers", &Request::getHeader);
			//reg("type", &Request::reqType);
			//reg("getPost", &Request::getPost);

			reg("writeHead", &Request::writeHead);
			reg("write", &Request::writeReq);
			reg("end", &Request::endReq);

			uv_tcp_init(parent->loop, handle);
			http_parser_init(&parser, HTTP_REQUEST);
			handle->data = this;
			parser.data = this;
		}

		void close() {
			if(!reqOpen) return;
			reqOpen = false;
			debug(4,	"close req") ;
			/*
				cerr << "before mutex lock\n";
				// needs to be exclusive lock
				close_mutex.lock();
				/*
				while(!close_mutex.try_lock()) {
				//close_mutex.unlock();
				//uv_run_once(parent->loop);
				}
				cerr << "after mutex lock\n";
				// this does not check if there are pending write calls
				// and if they are they will end up getting canceled
				cerr << "closing request\n";
				uv_close((uv_handle_t*)&handle, [](uv_handle_t *handle) {
				Request *req = (Request*) handle->data;
				req->reqOpen = false;
				req->close_mutex.unlock();
				});
			*/
			struct closeReq {
				Request *parent;
				uv_write_t req;
				uv_buf_t buf;
				uv_tcp_t* handle;
			};
			closeReq *req = new closeReq;
			req->parent = this;
			req->req.data = req;
			req->buf.len = 0;
			req->buf.base = "\0";
			req->handle = handle; // in case the class is deleted
			uv_write(&req->req, (uv_stream_t*)handle, &req->buf, 1,
							 [](uv_write_t *req, int status) -> void {
								 // the writes are performed in order, so this will be called after the other writes have finished
								 closeReq *closereq = (closeReq*) req->data;
								 // apears that uv cares about the address of the handle
								 uv_close((uv_handle_t*)closereq->handle, [](uv_handle_t *handle) {
										 uv_tcp_t *hh = (uv_tcp_t*)handle;
										 delete hh;
										 // do not think that I need anything when closing it as everything is cleaned up else where
									 });
								 /*uv_close((uv_handle_t*)&closereq->parent->handle, [](uv_handle_t *handle) {
								 // do not think that I need anything when closing it as everything is cleaned up else where
								 });
								 */
								 delete closereq;
							 });
		}
	public:
		Request(Server *p /* some data from the request */) {
			parent = p;

			Init();
		}

		virtual ~Request() {
			// need to think something up for dealing with the close case
			close();

			//assert(0);
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
		debug(4, "Server got a connection" );
		Server *ser = (Server*)server->data;
		assert(ser);
		assert((uv_tcp_t*)server == &ser->server);
		//ser->on_connection(server, status);
		Request *req = new Request(ser);
		int r = uv_accept(server, (uv_stream_t*)req->handle);
		assert(!r);
		uv_read_start((uv_stream_t*)req->handle, on_alloc, on_read_cb);
	}

	void Server::on_read_cb(uv_stream_t *stream, ssize_t nread, uv_buf_t buf) {
		debug(4, "Server got read data" );
		Request *req = (Request*) stream->data;
		if(nread >= 0) {
			size_t parsed = http_parser_execute(&req->parser, &req->parent->parser_settings, buf.base, nread);
			if(parsed < nread) {
				// log some error here or something
				assert(0);
				uv_close((uv_handle_t*)stream, on_close);
			}
		}else{
			// some error with libuv
			assert(0);
		}
	}

	void Server::on_close(uv_handle_t *handle) {
		Request *req = (Request*) handle->data;

		debug(4, "request closed" );

		// at this point we should set it to closed or something
	}

	int Server::header_complete_cb(http_parser *parser) {
		Request *req = (Request*) parser->data;
		debug(4, "got to point where headers are done" );
		// believe that request is ready at this point
		//auto rr_ = make_handle<ilang::Object>(req);
		ValuePass rr = valueMaker(true);
		// at this point Request will be deleted when rr is lost
		vector<ValuePass> params = {rr};
		//ValuePass ret = ValuePass(new ilang::Value);
		Function & func = req->parent->callback;
		/*if(callback->native) {
			function->ptr(NULL, params, &ret);
			}else */
		// need to create a thread here and execuit the code

		Context ctx; // this is a callback so there isn't really any context
		ValuePass ret = func(ctx, params);
		// if(func.object) {
		// 	assert(func.object->Get().type() == typeid(ilang::Object*));
		// 	ScopePass obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(func.object->Get())));
		// 	func.ptr(obj_scope, params, &ret);
		// }else{
		// 	func.ptr(ScopePass(), params, &ret);
		// }

		// ignore the return type here

		return 1;
	}

	int Server::url_cb(http_parser *parser, const char *dat, size_t length) {
		std::string url(dat, length);
		Request *req = (Request*)parser->data;
		req->url = url;
		//cout << "url: " << url << endl;
		return 0;
	}

	int Server::header_field_cb(http_parser *parser, const char *dat, size_t length) {
		std::string field(dat, length);
		boost::algorithm::to_lower(field);
		Request *req = (Request*) parser->data;
		req->last_header = field;
		//cout << "field: " << field << endl;
		return 0;
	}

	int Server::header_value_cb(http_parser *parser, const char *dat, size_t length) {
		std::string value(dat, length);
		//cout << "header value: " << value << endl;
		Request *req = (Request*) parser->data;
		req->headers[req->last_header] = value;
		return 0;
	}

	/*void on_connection (uv_stream_t *server, int status) {

		}*/

	ValuePass Server::isRunning(Context &ctx, Arguments &args) {
		error(args.size() == 0, "httpd.running does not take any arguments");
		return valueMaker(listening);
	}

	ValuePass Server::setListen(Context &ctx, Arguments &args) {
		error(args.size() == 1, "http.setListen takes 1 argument");
		error(args[0]->type() == typeid(ilang::Function), "http.setListen takes a function for an argument");

		_callback = args[0];
		callback = *args[0]->cast<Function*>();

		return ValuePass();
	}

	ValuePass Server::startListen(Context &ctx, Arguments &args) {
		error(args.size() <= 1, "httpd.start takes one argument");
		error(listening == false, "httpd already listening");

		if(args.size()) {
			error(args[0]->type() == typeid(long), "argument to httpd.start expected to be a number");
			listenPort = args[0]->cast<int>();
		}

		// fill in
		debug(4, "starting listen" );
		setUpUV(listenPort);
		int r = uv_listen((uv_stream_t*)&server,
											SERVER_QUEUE_BUFFER_SIZE,
											on_connection_cb);
		uv_run(loop, UV_RUN_DEFAULT);
		if(r != 0) { cerr << "Problem opening up socket\n"; }
		return valueMaker(bool(r==0));

	}
	ValuePass Server::stopListen(Context &ctx, Arguments &args) {
		error(args.size()== 0, "httpd.stop takes no arguments");
		error(listening == true, "httpd not listening");

		// fill in
		// close the socket on loop

		return ValuePass();
	}

	ValuePass Server::waitEnd(Context &ctx, Arguments &args) {
		assert(0);
	}

	void Server::Init() {
		reg("running", &Server::isRunning);
		reg("listen", &Server::startListen);
		reg("stop", &Server::stopListen);
		reg("setCallback", &Server::setListen);
		reg("wait", &Server::waitEnd);

		loop = uv_loop_new();

		parser_settings.on_headers_complete = header_complete_cb;
		parser_settings.on_url = url_cb;
		parser_settings.on_header_field = header_field_cb;
		parser_settings.on_header_value = header_value_cb;

		debug(4, "server created" );
	}

	Server::Server(ValuePass call, int port) {
		assert(call->type() == typeid(Function));
		assert(port >= 0 && port <= 0xFFFF);
		listening = false;
		_callback = call;
		callback = *call->cast<Function*>();
		listenPort = port;
		Init();
	}

	Server::~Server() {
		// running the loop when the server is deleted might be an issue
		// uv_run(loop);
		uv_loop_delete(loop);
	}

	// Server end


	ValuePass createServer(Context &ctx, Arguments &args) {
		error(args.size() == 2, "httpd.create takes 2 arguments");
		error(args[0]->type() == typeid(long), "httpd.create 1st argument is a port number");
		error(args[1]->type() == typeid(ilang::Function), "httpd.create 2nd argument is a callback function");
		Server *ser = new Server(args[1], args[0]->cast<int>());
		//auto obj = make_handle<ilang::Object>(ser);
		return valueMaker(true);//obj);
	}
}

ILANG_LIBRARY_NAME("net/httpd",
									 ILANG_FUNCTION("create", createServer)
									 )
