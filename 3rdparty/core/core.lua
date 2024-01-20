include (path.join(SCRIPTS_DIR, "utility.lua"))

function core()
	add_target_static_library("core",
					{},
					{},
					{},
					{"rttr", "glm"},
					"c++",
					{},
					{})
end
core()