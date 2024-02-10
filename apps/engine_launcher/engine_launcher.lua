include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	add_target_app("engine_launcher",
					{"/bigobj"},
					{},
					{"Lua", "EASTL", "spdlog", "rttr", "core", "mimalloc", "raylib", "flecs"},
					{"test", "test2", "plugin_logging", "plugin_module_example", "plugin_kingdoms_core", "plugin_camera_system"},
					{},
					{"rapidjson", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge"},
					{}
					)
end
engine_launcher()