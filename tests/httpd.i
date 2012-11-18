from net import httpd
//from net import curl

main = {
	httpd.create(8080, {|req|
		Print("gotten a request from httpd");
		Print("\nReq: ",req);
		Print("\nUrl: ",req.url());
		req.writeHead(200, "Something random");
		req.write("hello world");
		req.end("Hello world");
		req=0;
	}).listen();
};
