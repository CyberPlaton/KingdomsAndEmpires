include (path.join(SCRIPTS_DIR, "utility.lua"))

function bx()
	project("bx")
		language ("c++")
		cppdialect "c++17"
		location (path.join(".project", "bx"))

		kind ("StaticLib")

		files{"bx/src/amalgamated.cpp"}
		includedirs{"bx/include", "bx/3rdparty"}

		if PLATFORM == "windows" then
			includedirs{"bx/include/compat/msvc"}
		elseif PLATFORM == "linux" then
			includedirs{"bx/include/compat/linux"}
		elseif PLATFORM == "macosx" then
			includedirs{"bx/include/compat/osx"}
		else
		end

		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()
		set_basic_defines()

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end
bx()