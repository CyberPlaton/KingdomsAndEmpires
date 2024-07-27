include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_ai()
	add_target_plugin("plugin_ai",
					{},
					{},
					{},
					{"spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy"},
					{},
					true)
end
plugin_ai()