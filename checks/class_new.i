test = class {
        a = 1;
};

main = {
        gg = test.new();
        assert(test.instance(gg));
        assert(test.interface(gg));
};
