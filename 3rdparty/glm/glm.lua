include (path.join(SCRIPTS_DIR, "utility.lua"))

function glm()
	add_target_library("glm",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
glm()