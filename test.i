something = class {
	test: 123,
	aa: {
		Print("from the class");
	}
};

main = {
	what = new(something);
	Print("working");
	Print(what.test);
	what.aa();
};
