from i import Import

main = {
	mod = Import.get("i.mod");
	mod.file(mod.self).each({|dat|
		Print("called back\n", dat, "  Name:", dat.name(), "  code:", dat.print());
		
	});
};
