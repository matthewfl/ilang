from i import mod

test = {
	Print("a print from the test function\n");
};

main2 = {
	s = mod.file(mod.self);
	Print(s, "\n");
	if(s.type(mod.type.File)) {
		l = s.list();
		Print(l.length);
	}
};


main = {
	a = [1,2,3];
	Print(a, " ", a[1], " ", a.push, " ", a.length);
	a.push(4);
	Print(a.length, " ", a[3]);
};
