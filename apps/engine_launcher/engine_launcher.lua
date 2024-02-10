include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	options = {""}

	if PLATFORM == "windows" then
		options = {"/bigobj"}
	end

	-- Note: raylib is linked automatically for apps and plugins,
	-- for architectural reasons define as header only
	add_target_app("engine_launcher",
					options,
					{},
					{"Lua", "EASTL", "spdlog", "rttr", "core", "mimalloc", "flecs"},
					{"test", "test2", "plugin_logging", "plugin_module_example", "plugin_kingdoms_core", "plugin_camera_system", "plugin_race_human"},
					{},
					{"rapidjson", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "raylib"},
					{}
					)
end
engine_launcher()