include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_module_example()
	add_target_plugin("plugin_module_example",
					{},
					{},
					{"plugin_logging"},
					{"spdlog", "core", "flecs", "spritemancer", "spritemancer", "unittest"},
					{},
					{"nlohmann", "magic_enum", "taskflow", "argparse", "glm", "Tracy"},
					{},
					true)
end
plugin_module_example()