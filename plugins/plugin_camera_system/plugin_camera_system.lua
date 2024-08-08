include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_camera_system()
	add_target_plugin("plugin_camera_system",
					{},
					{},
					{},
					{"Lua", "spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "raylib", "imgui"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy", "asio"},
					{"../../3rdparty/rttr/rttr/src"},
					true)
end
plugin_camera_system()