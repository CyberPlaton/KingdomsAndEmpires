include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_logging()
	add_target_library("plugin_logging",
					{},
					{},
					{},
					{"rttr", "glm", "EASTL", "magic_enum", "taskflow", "spdlog", "box2d", "spdlog", "core", "mimalloc"},
					false,
					"c++")
end
plugin_logging()