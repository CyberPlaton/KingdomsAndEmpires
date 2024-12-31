include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_kingdoms_core()
	add_target_plugin("plugin_kingdoms_core",
					{},
					{},
					{},
					{"spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "imgui", "bimg", "bx", "bgfx", "glfw"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "asio"},
					{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"},
					true)
end
plugin_kingdoms_core()
