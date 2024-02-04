include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	add_target_app("engine_launcher",
					{},
					{},
					{"Lua", "LuaBridge", "rapidjson", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "rttr", "core", "mimalloc", "raylib", "flecs"},
					{"test", "test2", "plugin_logging", "plugin_module_example", "plugin_kingdoms_core"})
end
engine_launcher()