include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_module_example()
	add_target_plugin("plugin_module_example",
					{},
					{},
					{"plugin_logging", "test", "test2"},
					{"glm", "EASTL", "magic_enum", "taskflow", "sokol", "box2d", "spdlog", "core", "mimalloc", "flecs"},
					{},
					true)
end
plugin_module_example()