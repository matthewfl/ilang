test = {
        {
                return 5;
        }
        return 10;
};

main = {
        assert(test() == 5);
};
