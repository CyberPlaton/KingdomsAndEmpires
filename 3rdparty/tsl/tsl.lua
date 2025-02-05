include (path.join(SCRIPTS_DIR, "utility.lua"))

function tsl()
	name = "tsl"
	build_options = {}
	define_flags = {"tsl_EXPORTS"}
	tsl_path = "tsl/src"
	thirdrdparty_path = path.join(tsl_path, "3rdparty")
	additional_includes = {tsl_path, thirdrdparty_path}

	project(name)
		language ("c++")
		location (path.join(".project", name))
		targetname (name)

		kind ("SharedLib")

		files{
			tsl_path .. "/**.hpp",
			tsl_path .. "/**.h",
			tsl_path .. "/**.cpp",
			tsl_path .. "/**.c",
		}

		externalincludedirs {additional_includes}
		includedirs {additional_includes}
		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		set_include_path_to_self(name)
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()
		configure()
end
tsl()