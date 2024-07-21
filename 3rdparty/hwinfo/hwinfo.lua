include (path.join(SCRIPTS_DIR, "utility.lua"))

function hwinfo()
	project("hwinfo")
		language ("c++")
		location (path.join(".project", "hwinfo"))

		kind ("StaticLib")

		files{"hwinfo/src/**.cpp"}

		if PLATFORM == "windows" then
			links{"wbemuuid"}
		elseif PLATFORM == "macosx" then
			links{"IOKit", "CoreFoundation"}
		else
		end

		externalincludedirs{"hwinfo/include"}
		set_basic_defines()
		set_include_path_to_self("hwinfo")
		set_include_path_to_engine()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		configure()
end
hwinfo()