include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer_testbed()
	add_target_app("spritemancer_testbed",
					{},
					{},
					{"Lua", "EASTL", "spdlog", "rttr", "core", "mimalloc", "flecs", "sdl", "bx", "bimg", "spritemancer"},
					{"plugin_logging"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{},
					true -- bx includes
					)
end
spritemancer_testbed()