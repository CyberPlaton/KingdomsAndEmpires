include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_kingdoms_and_empires()
	add_target_plugin("plugin_kingdoms_and_empires",
					{},
					{},
					{"plugin_kingdoms_core", "plugin_camera_system", "plugin_race_human"},
					{"Lua", "spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "raylib", "imgui"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{"../../3rdparty/rttr/rttr/src"},
					true)
end
plugin_kingdoms_and_empires()