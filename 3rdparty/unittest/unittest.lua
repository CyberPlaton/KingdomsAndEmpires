include (path.join(SCRIPTS_DIR, "utility.lua"))

function unittest()
	name = "unittest"
	build_options = {"-fPIC"}
	define_flags = {"UNITTEST_DISABLE_SHORT_MACROS"}

	project(name)
		language ("c++")
		location (path.join(".project", name))
		targetname (name)

		kind ("StaticLib")

		-- UnitTest++ required files
		files{"unittest/UnitTest++/*.h",
			  "unittest/UnitTest++/*.cpp"
			}

		if PLATFORM == "windows" then
			defines{"CRT_SECURE_NO_DEPRECATED"}
			files{"unittest/UnitTest++/Win32/**.h",
				  "unittest/UnitTest++/Win32/**.cpp"
			}
		end
		if PLATFORM ~= "windows" then
			defines{}
			files{"unittest/UnitTest++/Posix/**.h",
				  "unittest/UnitTest++/Posix/**.cpp"
			}
		end

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		set_include_path_to_self(name)
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		if IS_WARNING_AND_ERRORS_ENABLED == false then
			set_and_disable_common_warnings_errors()
		end

		configure()
end
unittest()
