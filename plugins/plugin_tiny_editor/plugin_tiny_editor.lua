include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_tiny_editor()
	add_target_plugin("plugin_tiny_editor",
					{},
					{},
					{"plugin_logging", "plugin_kingdoms_core", "plugin_camera_system", "plugin_race_human", "plugin_kingdoms_and_empires"},
					{"Lua", "EASTL", "spdlog", "rttr", "core", "mimalloc", "flecs"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "raylib", "Tracy"},
					{},
					true)
end
plugin_tiny_editor()