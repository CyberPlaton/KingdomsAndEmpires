include (path.join(SCRIPTS_DIR, "utility.lua"))

function fcpp()
	project("fcpp")
		language ("c")
		location (path.join(".project", "fcpp"))

		kind ("StaticLib")

		files { "fcpp/cpp1.c",
		"fcpp/cpp2.c",
		"fcpp/cpp3.c",
		"fcpp/cpp4.c",
		"fcpp/cpp5.c",
		"fcpp/cpp6.c"}

		includedirs { "fcpp" }

		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))

		configure()
end
fcpp()