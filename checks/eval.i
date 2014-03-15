import i.eval


main = {
	String code = "return \"Hello world\";";

	Print("Running: ", code);

	Function f = i.eval.eval(code);
	assert(f() == "Hello world");

	code = "return x * x;";
	f = i.eval.eval("x", code);

	assert(f(.5) == .25);
};
