include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	add_target_app("engine_launcher",
					{},
					{},
					{"Lua", "LuaBridge", "rapidjson", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "rttr", "core", "mimalloc", "raylib", "flecs", "argparse"},
					{"test", "test2", "plugin_logging", "plugin_module_example", "plugin_kingdoms_core", "plugin_camera_system"}
					)
end
engine_launcher()