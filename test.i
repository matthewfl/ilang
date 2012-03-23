what = {
Print("something else that could go here");
return 1;
};
something = {
       Print( what() );  
        Print("other\n");
};

main = {
        Print("Test	");
	something();
};
