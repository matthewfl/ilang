some = class {
	init: {},
	words: "hello this world",
	say: {
		Print("in say");
		Print(words);
	}
};

ob = object {

	a : {
		Print("2222222");
	}
};

main = {
	ob.a();
	aaa = new(some);
	aaa.say();
};
