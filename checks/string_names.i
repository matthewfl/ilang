main = {
        a = "test";
        gg = object {
                test: 123,
                another: 456
        };
        assert(123 == gg[a]);
};
