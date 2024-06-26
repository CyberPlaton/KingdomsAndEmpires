include (path.join(SCRIPTS_DIR, "utility.lua"))

function unittest()
	name = "unittest"
	build_options = {}
	define_flags = {}

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
			defines{"-Wall -Wextra -Werror"}
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

		configure()
end
unittest()