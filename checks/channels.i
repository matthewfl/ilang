from i import channel

global_count = 0;

ret_ch = channel.create(0);

runner1 = {
	|ch|
	assert(ch.pop() == 123);
	assert(global_count == 0);
	global_count = 1;
	ret_ch.push(333);
};

runner2 = {
	|ch|
	assert(ch.pop() == 567);
	assert(global_count == 2);
	global_count = 3;
	ret_ch.push(555);
};


main = {
	ch1 = channel.create(0);
	ch2 = channel.create(0);
	go(runner1, ch1);
	go(runner2, ch2);
	ch1.push(123);
	num1 = ret_ch.pop();
	Print(num1);
	global_count = 2;
	assert(num1 == 333);
	ch2.push(567);
	assert(ret_ch.pop() == 555);
	assert(global_count == 3);
};
