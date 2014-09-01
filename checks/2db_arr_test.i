DB("arr_test") arr_test = [];

main = {
	// to be run after 1db_arr_test
	assert(arr_test.length == 3);
	Print(arr_test);
	arr_test.push(4);
	Print(arr_test);
	assert(arr_test.length == 4);
};
