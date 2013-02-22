from i import map

Object a = object {
	a: 1,
	b: 2,
	c: 3
};

main = {	
	Print(a);
	list = map.create(a).map({|key, value, emit|
		Print("key ", key, " value: ", value, "\n");
		emit("k", key);	
	}).get("k");

	Print("list: ", list);
};
