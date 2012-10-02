
main = {
	a = [1,2,3];
	assert(a[0] == 1 && a[1] == 2 && a[2] == 3 && a.length == 3);
	PrintArray(a);
};


PrintArray = {|arr|
	Array a = arr;
	Int i = 0;
	Print("[");
	while(i < arr.length) {
		Print(arr[i], " ");
		i = i + 1;
	}
	Print("]");
};
