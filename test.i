import i.Import
import i.test
from net import curl

a_class = class {
	eeee: {
		Print("the eeee function from the class that is created in i\n");
	}
};

main = {
	Print("Running");
	Print(i);
	Print(i.Import);
	Print(i.Import.get);
	i.Import.get();

	Print("\n\ntry 2\n");
	Print(i.test.ttt);
	i.test.ttt();
	Print(i.test.wwww);
	another = new(a_class);
	another.eeee();
	something = new(i.test.wwww);
	Print(something);
	Print(something.eeee);

	something.eeee();

	Print("\n\n\ntryin curl\n");
	Print(curl.simpleGet);
	Print(curl.simpleGet("http://www.google.com/"));
};
