main = {
	mod = import("i.mod");
	mod.file(mod.self).each({|dat|
		Print("called back\n", dat, "  Name:", dat.name(), "  code:", dat.str());
		
	});
	Print("main node: ", mod.node(main).str());
	mod.node(main).each({|dat|
		Print("\n\tfrom inside the main: ", dat.str());
	});
};
