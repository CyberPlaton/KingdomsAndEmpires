include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_static_library("engine",
		{},
		{},
		{},
		{"rttr", "glm", "EASTL", "magic_enum", "taskflow", "box2d", "spdlog", "core", "mimalloc", "sokol", "flecs"},
		"c++",
		{},
		{"test", "test2", "plugin_logging"})
end
engine()