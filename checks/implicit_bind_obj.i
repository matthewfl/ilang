gg = object {
        test: {
                return q;
        },
        q: 123,
};

main = {
        assert(gg.test() == 123);
};
