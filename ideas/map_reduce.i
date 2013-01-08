from i import map

Db someData = {};
/* Note that there is no current way to get the keys from the database
This will allow for the data to not all be loaded on one machine
or have the data across a large network, or even be a data set that is too large to work with
*/

main = {
	map_object = map.create(someData); // must be of an object type (can be class, not array)
	map_second = map_object.map({|key, values, emit|
		// value is an array contain everything that is mapped to this value
		// when first coming out of an object, it is expected to be an array with only 1 object
		emit("string" + key, values[0]);
	});
	map_third = map_second.map({|key, values, emit|
		// values will at least be of length 1
		if(string.find(key, "string")) {
			emit(string.replace(key, "string", ""), values[0]);
		}
	});
	map_fourth = map_third.reduce({|key, values, emit|
		// if key is emitted twice then one of the keys is chosen at "random"
		emit(key, ....);
	});
	map_third.get("key") == // array of values
	map_forth.get("key") == [] if nothing or [value] if there was something
	// ...
	

};



list_all_keys = {|Object obj|
	return map.create(obj).map({|key, values, emit|
		emit("k", key);
	}).get("k");
};

// the reduce function might end up looking more like:

map.reduce = {|Funciton fun|
	// A normal mapping process followed by something that throws away the extra values?
	return this.map(fun).map({|key, values, emit|
		emit(key, values[0]);
	});
};
