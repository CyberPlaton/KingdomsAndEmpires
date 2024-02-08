include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_camera_system()
	add_target_plugin("plugin_camera_system",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "rapidjson", "glm", "EASTL", "magic_enum", "taskflow", "raylib", "spdlog", "core", "mimalloc", "flecs", "argparse"},
					{},
					true)
end
plugin_camera_system()