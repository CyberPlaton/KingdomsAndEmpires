include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer_testbed()
	add_target_app("spritemancer_testbed",
					{},
					{},
					{"Lua", "EASTL", "spdlog", "core", "flecs", "spritemancer", "tree_sitter"},
					{"plugin_logging"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{},
					true -- bx includes
					)
end
spritemancer_testbed()