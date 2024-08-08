include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	add_target_app("engine_launcher",
					{},
					{},
					{"Lua", "spdlog", "core", "flecs", "slang", "spritemancer", "mimalloc", "hwinfo", "rttr", "raylib", "imgui"},
					{"plugin_module_example", "plugin_kingdoms_core", "plugin_camera_system", "plugin_ai",
					"plugin_status_effects", "plugin_race_human", "plugin_render_system"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{"../../3rdparty/rttr/rttr/src"}
					)
end
engine_launcher()