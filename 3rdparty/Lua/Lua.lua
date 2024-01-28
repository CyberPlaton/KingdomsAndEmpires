include (path.join(SCRIPTS_DIR, "utility.lua"))

function Lua()
	if VERBOSE == true then
		print("\tstatic library: Lua")
	end
	project("Lua")
		language ("c")
		location (path.join(".project", "Lua"))

		kind ("StaticLib")

		files{"src/onelua.c"}

		set_basic_defines()
		set_include_path_to_self("Lua")
		set_include_path_to_engine()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end
Lua()