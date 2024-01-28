include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_library_ex("engine",
		{}, -- build_options
		{}, -- define_flags
		{"plugin_logging"}, -- plugin_deps
		{"glm", "EASTL", "magic_enum", "taskflow", "box2d", "spdlog", "core", "mimalloc", "sokol", "flecs"}, -- thirdparty_deps
		false,
		"c++",
		{}) -- thirdparty_headeronly_deps
end
engine()