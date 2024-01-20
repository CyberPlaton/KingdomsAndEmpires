include (path.join(SCRIPTS_DIR, "utility.lua"))

function magic_enum()
	add_target_library("magic_enum",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
magic_enum()