include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_module_example()
	add_target_plugin("plugin_module_example",
					{},
					{},
					{"plugin_logging", "test", "test2"},
					{"rapidjson", "rttr", "glm", "EASTL", "magic_enum", "taskflow", "raylib", "spdlog", "core", "mimalloc", "flecs"},
					{},
					true)
end
plugin_module_example()