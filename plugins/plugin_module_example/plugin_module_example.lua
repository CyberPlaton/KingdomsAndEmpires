include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_module_example()
	add_target_plugin("plugin_module_example",
					{},
					{},
					{},
					{"spdlog", "core", "flecs", "spritemancer", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "imgui", "bimg", "bx", "bgfx", "glfw"},
					{},
					{"nlohmann", "magic_enum", "taskflow", "argparse", "glm", "asio"},
					{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"},
					true)
end
plugin_module_example()
