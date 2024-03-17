include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer()
	add_target_static_library("spritemancer",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "EASTL", "spdlog", "core", "mimalloc", "sdl", "bgfx", "bx", "bimg"},
					"c++",
					{}, -- plugin_headeronly_deps
					{"nlohmann", "glm", "magic_enum", "taskflow", "Tracy"}, -- thirdparty_headeronly_deps
					{}, -- additional_includes
					true -- bx includes
					)
end
spritemancer()