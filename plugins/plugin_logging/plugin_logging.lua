include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_logging()
	add_target_plugin("plugin_logging",
					{},
					{},
					{},
					{"cereal", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "core", "mimalloc"},
					{})
end
plugin_logging()