from i import map

tests = object {
        a: 123,
        b: 456
};

main = {
        //       Print("hello world");
        map.create(tests).map({|key, value, emit|
                Print(key, ": ", value);
        });
};
