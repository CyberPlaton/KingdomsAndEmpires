include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_logging()
	add_target_plugin("plugin_logging",
					{},
					{},
					{},
					{"spdlog", "core", "mimalloc"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy"},
					{},
					false)
end
plugin_logging()