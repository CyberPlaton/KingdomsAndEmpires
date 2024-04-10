include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_ai()
	add_target_plugin("plugin_ai",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "EASTL", "spdlog", "core", "mimalloc", "flecs"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy", "raylib"},
					{},
					true)
end
plugin_ai()