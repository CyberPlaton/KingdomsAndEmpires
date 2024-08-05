include (path.join(SCRIPTS_DIR, "utility.lua"))

function flecs()
	name = "flecs"
	define_flags = {"flecs_EXPORTS"}

	project(name)
		language ("c")
		location (path.join(".project", name))
		targetname (name)

		kind ("SharedLib")

		files{"flecs/flecs.c"
			}

		if PLATFORM == "windows" then
			links{"wsock32", "ws2_32"}
		else
			links{"pthread"}
		end

		set_basic_defines()
		defines{define_flags}
		set_include_path_to_self(name)
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()
		configure()
end
flecs()