include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_kingdoms_core()
	add_target_plugin("plugin_kingdoms_core",
					{},
					{},
					{"plugin_logging"},
					{"spdlog", "core", "flecs", "spritemancer"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy"},
					{},
					true)
end
plugin_kingdoms_core()