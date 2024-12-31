include (path.join(SCRIPTS_DIR, "utility.lua"))

function kingdoms_and_empires()
	-- Create the project with main entry point application
		add_target_project("kingdoms_and_empires",
		{},
		{},
		{"plugin_camera_system"},
		{"Lua", "spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "imgui", "bimg", "bx", "bgfx", "glfw"},
		{},
		{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "asio"},
		{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"})
end
kingdoms_and_empires()