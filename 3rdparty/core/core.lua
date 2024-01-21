include (path.join(SCRIPTS_DIR, "utility.lua"))

function core()
	add_target_static_library("core",
					{},
					{"CORE_USE_EASTL", "core_EXPORTS"},
					{},
					{"rttr", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "box2d", "mimalloc"},
					"c++",
					{},
					{})
end
core()