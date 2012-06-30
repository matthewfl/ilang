something = object {
	what: "test",
	other: {
		Print("The other function\" worked");
	}
};

main = {
	Print("This is getting printed out from the object: " , something.what, "\n");
	something.other();
};
