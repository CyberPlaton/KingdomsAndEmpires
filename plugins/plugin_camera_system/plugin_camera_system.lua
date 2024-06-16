include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_camera_system()
	add_target_plugin("plugin_camera_system",
					{},
					{},
					{"plugin_logging"},
					{"Lua", "spdlog", "core", "flecs", "spritemancer"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy"},
					{},
					true)
end
plugin_camera_system()