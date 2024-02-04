include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_kingdoms_core()
	add_target_plugin("plugin_kingdoms_core",
					{},
					{},
					{"plugin_logging"},
					{"cereal", "glm", "EASTL", "magic_enum", "taskflow", "raylib", "spdlog", "core", "mimalloc", "flecs"},
					{},
					false)
end
plugin_kingdoms_core()