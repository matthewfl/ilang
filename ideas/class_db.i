from i import db

// how to make database for classes work still needs some though
// as there is the whole issue with the keyword new and such
// maybe the variable that get set from the new needs to be the one that has the database set on it

something = class {
	Db Int test: 1,
	count: {
		test = test + 1;
		Print("the count: ", test);
	}
};

// if variables are there I don't recall if it just references the class and then saves a local copy or something in the object when it is changed, will need to check that
Db another = new(something); // another could load the saved version of the Db variables, and then the functions could be taken from this class


main = {
	
};
