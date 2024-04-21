include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	-- Note: raylib is linked automatically for apps and plugins,
	-- for architectural reasons define as header only
	add_target_app("engine_launcher",
					{},
					{},
					{"Lua", "EASTL", "spdlog", "core", "mimalloc", "flecs", "slang"},
					{"test", "test2", "plugin_logging", "plugin_module_example",
					"plugin_kingdoms_core", "plugin_camera_system", "plugin_ai",
					"plugin_status_effects", "plugin_race_human", "plugin_render_system"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{}
					)
end
engine_launcher()