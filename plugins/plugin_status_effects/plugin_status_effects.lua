include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_status_effects()
	add_target_plugin("plugin_status_effects",
					{},
					{},
					{"plugin_logging", "plugin_kingdoms_core"},
					{"EASTL", "spdlog", "core", "mimalloc", "flecs"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy"},
					{},
					true)
end
plugin_status_effects()