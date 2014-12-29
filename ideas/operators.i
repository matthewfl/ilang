// currently ilang does not have support for operators or casting support? should that be changed?

Eq = {|a| {|b| a == b; }};

test = class {
        cast = {|Eq(String) _|
                return "The string value of the test class";
              } + {|Eq(Int) _|
                return a;
              };
        equals = {|instance other|
                return a == other.a;
        };
        greater_than = {|instance other| a > other.a; };
        init = {|b| this.a = b; }
};

main = {
        gg = test.new(5);
        bb = test.new(6);
        assert(gg != bb); // using equals
        assert(bb > gg); // using greather_than
        Int ss = gg; // using cast(Int)
        assert(ss == 5);
};
