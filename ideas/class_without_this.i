something = class {
	test: 123,
	aa: {|self|
		Print("from the class: ", self.test);
	}
};

main = {
	what = new(something);
	Print("working");
	Print(what.test);
	what.aa(what);
};
