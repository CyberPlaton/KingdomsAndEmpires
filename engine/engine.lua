include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_library_ex("engine",
		{}, -- build_options
		{"B2_SHARED", "box2d_EXPORTS"}, -- define_flags
		{"plugin_logging"}, -- plugin_deps
		{"Lua", "LuaBridge", "cereal", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "core", "mimalloc", "sokol", "flecs"}, -- thirdparty_deps
		false,
		"c++",
		{"src/physics"}) -- additional_includes
end
engine()