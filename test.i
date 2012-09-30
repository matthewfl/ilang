from i import mod

test = {
	Print("a print from the test function\n");
};

main = {
	s = mod.file(mod.self);
	Print(s, "\n");
	if(s.type(mod.type.File)) {
		l = s.list();
		Print(l.length);
	}
};
