include (path.join(SCRIPTS_DIR, "utility.lua"))

function sokol()
	add_target_library("sokol",
					{},
					{},
					{},
					{},
					true,
					"c")
end
sokol()