from i import map

a = object {
        a: 123,
};

main = {
        map.create(a).map({|key, value, emit|
                Print(key);
        });
};
