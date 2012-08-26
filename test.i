from i import db

main = {
	Print("the main program is running\n");
	Print(db.metaGet("test"));
	db.metaSet("test", "something new \n" + db.metaGet("test"));
};
