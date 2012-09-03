Db run_count = 0;
String message = "\tThis is the message to print\n";
String Db say = "";

Db obj_db = object {};

main = {
	Print("This progam has been run ", run_count, " times before\n");
	run_count = run_count + 1;
	Print("The current message:\n", say);
	say = say + message;
	//obj_db.test = 1;
	if(obj_db.test || true) {
		Print("obj count ", obj_db.test);
		obj_db.test = obj_db.test + 1;
	}else{
		obj_db.test = 1;
	}
};
