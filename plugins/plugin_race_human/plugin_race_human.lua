include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_race_human()
	add_target_plugin("plugin_race_human",
					{},
					{},
					{"plugin_logging", "plugin_kingdoms_core"},
					{"spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy"},
					{},
					true)
end
plugin_race_human()