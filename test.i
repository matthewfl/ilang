what = {
Print("something else that could go here");
return "returned";
Print("this should never show");
};
something = {
       Print( what() );  
        Print("other\n");
};

main = {
        Print("Test	");
	something();
};
