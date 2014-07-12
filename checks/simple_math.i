equal_self = {
	i = 5;
	assert(i == 5);
	assert(( i += 5) == 10);
	assert(i == 10);
	assert(( i *= 5) == 50);
	assert(i == 50);
	assert(( i -= 5) == 45);
	assert(i == 45);
	assert(( i /= 5) == 9);
	assert(i == 9);
};

main = {
	assert(5 + 5 == 10);
	assert(5 * 5 == 25);
	assert(5 / 5 == 1);
	assert(5 - 5 == 0);
	equal_self();

};
