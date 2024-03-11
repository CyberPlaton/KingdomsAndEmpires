include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	-- Note: raylib is linked automatically for apps and plugins,
	-- for architectural reasons define as header only
	add_target_app("engine_launcher",
					{},
					{},
					{"Lua", "EASTL", "spdlog", "rttr", "core", "mimalloc", "flecs"},
					{"test", "test2", "plugin_logging", "plugin_module_example",
					"plugin_kingdoms_core", "plugin_camera_system",
					"plugin_status_effects", "plugin_race_human"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "raylib", "Tracy"},
					{}
					)
end
engine_launcher()