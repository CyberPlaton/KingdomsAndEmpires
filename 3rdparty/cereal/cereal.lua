include (path.join(SCRIPTS_DIR, "utility.lua"))

function cereal()
	add_target_library("cereal",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
cereal()