include (path.join(SCRIPTS_DIR, "utility.lua"))

function kingdom_wars()
	-- Create the project with main entry point application
		add_target_project("kingdom_wars",
		{},
		{},
		{"plugin_camera_system"},
		{"Lua", "spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "imgui", "raylib", "tsl"},
		{},
		{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "asio", "pfd"},
		{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"})
end
kingdom_wars()