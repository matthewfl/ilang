from i import mod

main = {
	mod.file(mod.self).each({|dat|
		Print("called back\n", dat, dat.print());
		
	});
};
