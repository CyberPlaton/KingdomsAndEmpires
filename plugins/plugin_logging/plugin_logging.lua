include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_logging()
	add_target_library("plugin_logging",
					{},
					{},
					{},
					{"spdlog", "core", "rttr", "glm", "EASTL"},
					false,
					"c++")
end
plugin_logging()