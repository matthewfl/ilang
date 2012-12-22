from i import channel

cha = channel.create(0);

runner = {|ch|
	Print("Runner is going");
	Print(ch, ch.push);
	ch.push(123);
	Print("done pushing");
};


main = {
	Print("Channel test\n");
	ch = channel.create(0);
	Print("After channel created");
	go(runner, ch);
	Print("after go call");
	Print(ch.pop());
	//Print(ch.pop());	
};
