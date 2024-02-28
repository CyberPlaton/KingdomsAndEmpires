include (path.join(SCRIPTS_DIR, "utility.lua"))

function Tracy()
	add_target_library("Tracy",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
Tracy()