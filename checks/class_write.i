cc = class {
 a = 1;
 b = {|gg|
         a = gg;
        };
 c = {
        return a;
 };
};

main = {
	c1 = cc.new();
	c2 = cc.new();
	Print(c1.a);
	assert(c1.a == 1);
	c1.b(23);
	Print("\n", c1.a);
	assert(c1.a == 23);
	Print("\n", c2.a);
	assert(c2.a == 1);
	Print("\n", c2.c());
	assert(c2.c() == 1);
	c2.a = 44;
	Print("\n", c2.a, " ", c1.a);
	assert(c1.a == 23, c2.a == 44);
//	assert(1 == 1 && 0 == 0); // noticed intresting problem in the way that this is parsed
};
