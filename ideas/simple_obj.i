something = object {
	what: "test",
	other: {
		Print("The other function\" worked");
	}
};

main = {
	Print("This is getting printed out from the object: " , something.what, "\n");
	something.other();
	
	Print("\nTime for more test\n");
	something.what = "the new value of what";
	Print("The current value of what: ", something.what);
	
	Print("\nSecond new test\n");
	something.new_value = 123;
	Print("The new value is: ", something.new_value);
	something.new_value = 456;
	Print("again: ", something.new_value);
	
	a = 5;
	while(a > 0) {
		a = a - 1;
		Print(a, "\n");
	}

};
