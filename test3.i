from i import map
main = {
        gg = object { a: 1 };
        q = map.create(gg);
        q.map({|key, value, emit|
                Print(key);
                emit(key, [value, 123]);
        }).reduce({|key, values, emit|
                Print(key);
                Print(values);
        });
};
