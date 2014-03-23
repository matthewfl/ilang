Const Class Type_int = class {
 validate: {
	 |t|
	 return typeof(t) == "Int"
 }
};

Class example = class {
 say: {
	 print("hello world\n");
 }
};

fun = {
	|class {Function say: {}}.instance item|
	item.say();
};


even = {
	|{|Int i| return i % 2 == 0; } i|
	print("The value of i is ", i, " and it must be even");
};

one_of = i.match
	(
	 even,
	 {|{|Int i| return i % 2 == 1; } i|
		print("the value is odd");
	 },
	 {|i|
			 // it wasn't even or odd so was not an Int
	 }
	 );
main = {
	fun(new(example));
	even(2);
};
