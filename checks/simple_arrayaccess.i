main = {
	a = [1,2,3];
	b = object {
		a: 1,
		c: 2,
		d: {
			Print(c, arguments.length);
		}
	};
	
	b["d"]("hello");
};
