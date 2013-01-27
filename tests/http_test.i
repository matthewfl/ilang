from net import curl
from net import httpd


server = {
	serv = httpd.create(8888, {|req|
		req.writeHead(200);
		req.write("Start of the page\n");
		req.write(req.url());
		req.end();
		serv.stop();
	});
	serv.listen();
};

client = {
	Print("client");
	dat = curl.simpleGet("http://localhost:8888/this_page");
	Print("client data: ", dat);
};

main = {
	go(server);
	go(client);
};
