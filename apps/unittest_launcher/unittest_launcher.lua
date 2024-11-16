include (path.join(SCRIPTS_DIR, "utility.lua"))

-- Note: unittest launcher must include and link all libraries and plugins that are being tested
function unittest_launcher()
	add_target_app("unittest_launcher",
					{},
					{},
					{ "Lua", "spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "imgui", "raylib" },
					{ "plugin_camera_system", "plugin_ai"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy", "asio"},
					{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"},
					"ConsoleApp"
					)
end
unittest_launcher()
