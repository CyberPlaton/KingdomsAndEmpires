include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_ai()
	add_target_plugin("plugin_ai",
					{},
					{},
					{},
					{"spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "imgui", "raylib"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "asio"},
					{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"},
					true)
end
plugin_ai()
