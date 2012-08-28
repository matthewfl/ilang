/*from i import db

main2 = {
	Print("the main program is running\n");
	Print(db.metaGet("test"));
	db.metaSet("test", "something new \n" + db.metaGet("test"));
};
*/


Db test = 1; // make this first value be the default value or something like that
Db long_and_annoying_thing = 1;

main = {
	test = test + 1;
	long_and_annoying_thing = 2;	
	Print("test: ", test);
};
