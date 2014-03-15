DB arr_test = [];


main = {
	// to be run after 2db_arr_test
	assert(arr_test.length == 4);
	assert(arr_test.pop() == 4);
	assert(arr_test[0] == 1);
};
