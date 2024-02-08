include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_logging()
	add_target_plugin("plugin_logging",
					{},
					{},
					{},
					{"rttr", "EASTL", "spdlog", "core", "mimalloc"},
					{},
					{"rapidjson", "glm", "magic_enum", "taskflow", "argparse"},
					{},
					false)
end
plugin_logging()