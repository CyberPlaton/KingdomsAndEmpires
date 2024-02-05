include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_kingdoms_core()
	add_target_plugin("plugin_kingdoms_core",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "rapidjson", "glm", "EASTL", "magic_enum", "taskflow", "raylib", "spdlog", "core", "mimalloc", "flecs"},
					{},
					true)
end
plugin_kingdoms_core()