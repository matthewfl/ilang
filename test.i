what = {
Print("something else that could go here");
return 0;
Print("this should never show");
};
something = {
      	if( what() )  
        	Print("other\n");
};

main = {
Print("1+1=", 1+1);
//        Print("Test	");
//	something();

};
