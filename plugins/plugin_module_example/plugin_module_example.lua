include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_module_example()
	add_target_plugin("plugin_module_example",
					{},
					{},
					{},
					{"spdlog", "core", "flecs", "spritemancer", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "raylib", "imgui"},
					{},
					{"nlohmann", "magic_enum", "taskflow", "argparse", "glm", "Tracy"},
					{"../../3rdparty/rttr/rttr/src"},
					true)
end
plugin_module_example()