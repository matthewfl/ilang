gg = object {
        test = {
                return this.q;
        };
        q = 123;
};

main = {
        assert(gg.test() == 123);
};

