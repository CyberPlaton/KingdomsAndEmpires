include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_camera_system()
	add_target_plugin("plugin_camera_system",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "EASTL", "raylib", "spdlog", "core", "mimalloc", "flecs"},
					{},
					{"rapidjson", "glm", "magic_enum", "taskflow", "argparse"},
					{},
					true)
end
plugin_camera_system()