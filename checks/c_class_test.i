import i.test

main = {
        assert(i.test.ttt() == "the ttt function called");
        gg = i.test.wwww.new();
        assert(gg.eeee() == "the eeee function called");
        gg.set(123);
        assert(gg.get() == 123);
};
