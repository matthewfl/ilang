main = {
	Print("From function");
	while(1) {
		go({
			Print("from second thread");
		});
	}	
};
