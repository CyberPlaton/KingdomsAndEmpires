include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_module_example()
	add_target_plugin("plugin_module_example",
					{},
					{},
					{"plugin_logging", "test", "test2"},
					{"rttr", "glm", "EASTL", "magic_enum", "taskflow", "sokol", "box2d", "spdlog", "core", "mimalloc", "flecs"},
					{})
end
plugin_module_example()