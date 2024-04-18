include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer_testbed()
	-- Note: raylib is linked automatically for apps and plugins,
	-- for architectural reasons define as header only
	add_target_app("spritemancer_testbed",
					{},
					{},
					{"Lua", "EASTL", "spdlog", "rttr", "core", "mimalloc", "flecs", "sdl", "bgfx", "bx", "bimg"},
					{"plugin_logging"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy"},
					{},
					true -- bx includes
					)
end
spritemancer_testbed()