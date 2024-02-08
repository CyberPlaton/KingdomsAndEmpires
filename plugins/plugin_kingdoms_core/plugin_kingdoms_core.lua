include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_kingdoms_core()
	add_target_plugin("plugin_kingdoms_core",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "EASTL", "raylib", "spdlog", "core", "mimalloc", "flecs"},
					{},
					{"rapidjson", "glm", "magic_enum", "taskflow", "argparse"},
					{},
					true)
end
plugin_kingdoms_core()