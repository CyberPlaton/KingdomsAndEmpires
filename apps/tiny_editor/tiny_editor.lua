include (path.join(SCRIPTS_DIR, "utility.lua"))

function tiny_editor()
	add_target_app("tiny_editor",
					{},
					{},
					{"Lua", "spdlog", "core", "flecs", "spritemancer", "mimalloc", "hwinfo", "rttr"},
					{"plugin_module_example", "plugin_kingdoms_core",
					"plugin_camera_system", "plugin_race_human", "plugin_kingdoms_and_empires",
					"plugin_status_effects", "plugin_tiny_editor", "plugin_render_system"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{}
					)
end
tiny_editor()