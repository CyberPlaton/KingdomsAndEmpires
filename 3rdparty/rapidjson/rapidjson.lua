include (path.join(SCRIPTS_DIR, "utility.lua"))

function rapidjson()
	add_target_library("rapidjson",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
rapidjson()