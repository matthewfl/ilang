from i import timer

back = {
	Print("from timer");
};

main = {
	Print(timer.setTimeout);
	timer.setTimeout(100, back);
	Print("After setting timer");
};
