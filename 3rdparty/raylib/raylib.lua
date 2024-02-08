include (path.join(SCRIPTS_DIR, "utility.lua"))

function raylib()
	project("raylib_project")
		language ("c")
		location (path.join(".project", "raylib_project"))

		kind ("None")

		set_basic_defines()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end
raylib()