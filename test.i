what = object {
	data: 123,
	call: {
		Print("from obj, ", this.data);
	},
	another: {
		Print("in object, no data\n"); 
	}
};


something = {
	Print(what.data);
	what.another();
	//what.call();
};

Const Int a = 5;

main = {
	//a = "hello";
	//a = 3;
	Print(what.this.this.this.this.data);
	//Print("main ");
	something();
};
