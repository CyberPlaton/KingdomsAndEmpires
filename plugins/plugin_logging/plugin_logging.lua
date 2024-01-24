include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_logging()
	add_target_plugin("plugin_logging",
					{},
					{},
					{},
					{"rttr", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "box2d", "spdlog", "core", "mimalloc"},
					{})
end
plugin_logging()