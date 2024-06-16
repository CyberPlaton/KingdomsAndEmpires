include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer_testbed()
	add_target_app("spritemancer_testbed",
					{},
					{},
					{"Lua", "spdlog", "core", "flecs", "spritemancer"},
					{"plugin_logging"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{},
					true -- bx includes
					)
end
spritemancer_testbed()