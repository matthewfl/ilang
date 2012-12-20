import i.test

qqq = class {
	aaa : {}
};

another = {
	Print("creating\n");
	gg = new(i.test.wwww);
	Print("destroying\n");
	gg = 0;
	Print("destroyed\n");

	Print("second\n");
	vv = new(qqq);
	Print("second delete\n");
	vv = 0;
	Print("second destroyed\n");
};

nn = {
	rr = new(i.test.wwww);
		
};

main2 = { nn(); another(); };

cc = {
	rr = new(i.test.wwww);
	rr.set(1);
	Print(rr.get());
};

main = { cc(); };
