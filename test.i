from i import mod

fun = {
	Print("Fun called\n");
	return object {
		h : {
			Print("h called");	
		}
	};
};

main = {
	Print("main\n");
	fun().h();
	Print("\n\n");
	obj = fun();
	obj.h();
};

main_aa = {
     //Print(mod.self);
     aa = mod.file(mod.self).Type();
	//aa.Type();
//    Print(mod.types.function);
};
