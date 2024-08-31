include (path.join(SCRIPTS_DIR, "utility.lua"))

function kingdoms_and_empires()
	-- Note: raylib is linked automatically for apps and plugins,
	-- for architectural reasons define as header only
	add_target_app("kingdoms_and_empires",
					{},
					{},
					{"Lua", "spdlog", "core", "flecs", "spritemancer", "mimalloc", "hwinfo", "rttr", "imgui", "bx", "bimg", "bgfx", "glfw"},
					{"plugin_module_example", "plugin_kingdoms_core", 
					"plugin_camera_system", "plugin_race_human", "plugin_render_system",
					"plugin_status_effects", "plugin_kingdoms_and_empires"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy", "asio"},
					{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"}
					)
end
kingdoms_and_empires()
