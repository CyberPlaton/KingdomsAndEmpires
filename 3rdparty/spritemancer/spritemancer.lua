include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer()
	-- TODO: after we are done with raylib, remove it from deps
	add_target_static_library("spritemancer",
					{},
					{},
					{"plugin_logging"},
					{"rttr", "EASTL", "spdlog", "core", "mimalloc", "sdl", "bgfx", "bx", "bimg"},
					"c++",
					{}, -- plugin_headeronly_deps
					{"nlohmann", "glm", "magic_enum", "taskflow", "Tracy", "raylib"}, -- thirdparty_headeronly_deps
					{}, -- additional_includes
					true -- bx includes
					)
end
spritemancer()