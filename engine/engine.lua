include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_static_library("engine",
		{}, -- build_options
		{}, -- define_flags
		{}, -- plugin_deps
		{"Lua", "spdlog", "core", "imgui", "raylib", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr"}, -- thirdparty_deps
		"c++",
		{}, -- plugin_headeronly_deps
		{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy", "asio"}, -- thirdparty_headeronly_deps
		{"src/physics", "../3rdparty/rttr/rttr/src"}, -- additional_includes
		true)
end
engine()
