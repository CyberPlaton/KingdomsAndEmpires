include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_module_example()
	add_target_plugin("plugin_module_example",
					{},
					{},
					{"plugin_logging", "test", "test2"},
					{"rttr", "EASTL", "raylib", "spdlog", "core", "mimalloc", "flecs"},
					{},
					{"rapidjson", "magic_enum", "taskflow", "argparse", "glm"},
					{},
					true)
end
plugin_module_example()