include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_static_library("engine",
		{}, -- build_options
		{}, -- define_flags
		{"plugin_logging"}, -- plugin_deps
		{"rttr", "Lua", "EASTL", "spdlog", "core", "mimalloc", "raylib", "flecs"}, -- thirdparty_deps
		"c++",
		{}, -- plugin_headeronly_deps
		{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"}, -- thirdparty_headeronly_deps
		{"src/physics"}) -- additional_includes
end
engine()