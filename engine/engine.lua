include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_static_library(ENGINE_NAME,
		{}, -- build_options
		{}, -- define_flags
		{}, -- plugin_deps
		{"Lua", "spdlog", "core", "imgui", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "raylib", "tsl"}, -- thirdparty_deps
		"c++",
		{}, -- plugin_headeronly_deps
		{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "asio"}, -- thirdparty_headeronly_deps
		{"src/physics", "../3rdparty/rttr/rttr/src", "../3rdparty/asio/include/asio/asio/include"}, -- additional_includes
		false)
end
engine()
