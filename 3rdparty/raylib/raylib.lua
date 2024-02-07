include (path.join(SCRIPTS_DIR, "utility.lua"))

function raylib()
	project("raylib")
		language ("c")
		location (path.join(".project", "raylib"))

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