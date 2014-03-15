
main = {
	a = [1,2,3];
	assert(a[0] == 1 && a[1] == 2 && a[2] == 3 && a.length == 3);
	PrintArray(a);
	a.push(4);

	assert(CompArray(a, [1,2,3,4]));
	assert(a.pop() == 4);
	assert(CompArray(a, [1,2,3]));
	Print(a);
	// need to finish off check of other array functions
};


PrintArray = {
	|Array arr|
	Print("[");
	for(Int i=0;i < arr.length;i = i + 1) {
		Print(arr[i], " ");
	}
	Print("]");
};

CompArray = {
	|Array arr1, Array arr2|
	if(arr1.length != arr2.length)
		return 0;
	for(Int i = 0; i < arr1.length; i = i + 1) {
		if(arr1[i] != arr2[i])
			return 0;
	}
	return 1;
};
