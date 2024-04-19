include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_render_system()
	add_target_plugin("plugin_render_system",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "EASTL", "spdlog", "core", "mimalloc", "flecs"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy"},
					{},
					true)
end
plugin_render_system()