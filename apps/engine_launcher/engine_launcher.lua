include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	add_target_app("engine_launcher",
					{},
					{},
					{"cereal", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "box2d", "spdlog", "core", "mimalloc", "sokol", "flecs"},
					{"test", "test2", "plugin_logging", "plugin_module_example"})
end
engine_launcher()