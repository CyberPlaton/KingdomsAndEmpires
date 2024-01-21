include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	add_target_app("engine_launcher",
					{},
					{},
					{"rttr", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "box2d", "spdlog", "core", "mimalloc", "sokol"},
					{"test", "test2", "plugin_logging"})
end
engine_launcher()