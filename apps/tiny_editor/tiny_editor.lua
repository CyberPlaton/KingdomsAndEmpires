include (path.join(SCRIPTS_DIR, "utility.lua"))

function tiny_editor()
	-- Note: raylib is linked automatically for apps and plugins,
	-- for architectural reasons define as header only
	add_target_app("tiny_editor",
					{},
					{},
					{"Lua", "EASTL", "spdlog", "rttr", "core", "mimalloc", "flecs"},
					{"test", "test2", "plugin_logging", "plugin_module_example", "plugin_kingdoms_core",
					"plugin_camera_system", "plugin_race_human", "plugin_kingdoms_and_empires",
					"plugin_tiny_editor"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "raylib", "Tracy"},
					{}
					)
end
tiny_editor()