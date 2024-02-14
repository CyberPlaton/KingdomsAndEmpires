include (path.join(SCRIPTS_DIR, "utility.lua"))

function nlohmann()
	add_target_library("nlohmann",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
nlohmann()