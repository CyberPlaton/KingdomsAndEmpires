include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_module_example()
	add_target_library("plugin_module_example",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "box2d", "spdlog", "core", "mimalloc", "flecs"},
					false,
					"c++")
end
plugin_module_example()