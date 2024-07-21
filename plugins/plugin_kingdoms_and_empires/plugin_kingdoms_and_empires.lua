include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_kingdoms_and_empires()
	add_target_plugin("plugin_kingdoms_and_empires",
					{},
					{},
					{"plugin_logging", "plugin_kingdoms_core", "plugin_camera_system", "plugin_race_human"},
					{"Lua", "spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{},
					true)
end
plugin_kingdoms_and_empires()