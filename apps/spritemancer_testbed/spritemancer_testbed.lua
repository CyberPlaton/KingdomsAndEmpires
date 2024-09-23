include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer_testbed()
	add_target_app("spritemancer_testbed",
					{},
					{},
					{"Lua", "spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "imgui", "raylib"},
					{"plugin_ai"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy", "asio"},
					{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"}
					)
end
spritemancer_testbed()
