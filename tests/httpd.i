from net import httpd
from net import curl

main = {
	httpd.create(8080, {|req|
		Print("gotten a request from httpd");
		Print("\nReq: "+req);
		Print("\nUrl: "+req.url());
		req.end("Hello world");
	}).listen();
};
