include (path.join(SCRIPTS_DIR, "utility.lua"))

function core()
	add_target_static_library("core",
					{},
					{"CORE_USE_EASTL"},
					{},
					{"rttr", "glm", "EASTL", "magic_enum", "taskflow"},
					"c++",
					{},
					{})
end
core()