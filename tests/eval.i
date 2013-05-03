from i import eval


stuff = {
        Print(eval.eval);
        Function aa = eval.eval("return 1;");
        Print(aa, aa());
        Print("after call\n");
        aa = {};
        Print("after null\n");
        Print("after null 2\n");
};


main = { 
        Print("before call\n");
        stuff();
        Print("before exit");
};
