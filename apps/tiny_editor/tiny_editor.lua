include (path.join(SCRIPTS_DIR, "utility.lua"))

function tiny_editor()
	add_target_app("tiny_editor",
					{},
					{},
					{"Lua", "spdlog", "core", "flecs", "spritemancer", "mimalloc", "hwinfo", "rttr", "imgui", "raylib"},
					{"plugin_module_example", "plugin_kingdoms_core",
					"plugin_camera_system", "plugin_race_human", "plugin_kingdoms_and_empires",
					"plugin_status_effects"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy", "pfd", "asio"},
					{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"}
					)
end
tiny_editor()
