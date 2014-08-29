from i import channel

global_count = 0;

ret_ch = channel.create(1);

runner1 = {
	|ch|
	//Print("From thread 1\n");
        assert(ch.pop() == 123);
	Print("Got message\n");
        assert(global_count == 0);
	global_count = 1;
	ret_ch.push(333);
};

runner2 = {
	|ch|
        //Print("From thread 2\n");
	assert(ch.pop() == 567);
	assert(global_count == 2);
	global_count = 3;
	ret_ch.push(555);
};


main = {
	ch1 = channel.create(1);
	ch2 = channel.create(1);
	Print("Starting threads\n");
        go(runner1, ch1);
	go(runner2, ch2);
	Print("sending msgs\n");
        ch1.push(123);
        Print("msg sent\n");
	num1 = ret_ch.pop();
	Print(num1);
	global_count = 2;
	assert(num1 == 333);
	ch2.push(567);
	assert(ret_ch.pop() == 555);
	assert(global_count == 3);
};
