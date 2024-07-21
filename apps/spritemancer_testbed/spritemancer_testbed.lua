include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer_testbed()
	add_target_app("spritemancer_testbed",
					{},
					{},
					{"Lua", "spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo"},
					{"plugin_logging", "plugin_ai"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{}
					)
end
spritemancer_testbed()