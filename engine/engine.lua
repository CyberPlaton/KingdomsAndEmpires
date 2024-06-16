include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_static_library("engine",
		{}, -- build_options
		{}, -- define_flags
		{"plugin_logging"}, -- plugin_deps
		{"Lua", "spdlog", "core", "flecs", "spritemancer"}, -- thirdparty_deps
		"c++",
		{}, -- plugin_headeronly_deps
		{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"}, -- thirdparty_headeronly_deps
		{"src/physics"}, -- additional_includes
		true)
end
engine()
