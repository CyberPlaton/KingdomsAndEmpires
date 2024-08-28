include (path.join(SCRIPTS_DIR, "utility.lua"))

function sdl()
	add_target_library("sdl",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
sdl()