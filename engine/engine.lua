include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_static_library("engine",
		{}, -- build_options
		{"engine_EXPORTS"}, -- define_flags
		{"plugin_logging"}, -- plugin_deps
		{"Lua", "LuaBridge", "cereal", "glm", "EASTL", "magic_enum", "taskflow", "box2d", "spdlog", "core", "mimalloc", "sokol", "flecs"}, -- thirdparty_deps
		"c++",
		{}, -- thirdparty_headeronly_deps
		{}) -- plugin_headeronly_deps
end
engine()