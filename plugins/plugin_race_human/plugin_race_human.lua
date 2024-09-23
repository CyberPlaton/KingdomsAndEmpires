include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_race_human()
	add_target_plugin("plugin_race_human",
					{},
					{},
					{"plugin_kingdoms_core"},
					{"spdlog", "core", "flecs", "spritemancer", "unittest", "mimalloc", "hwinfo", "rttr", "imgui", "raylib"},
					{},
					{"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "Tracy", "asio"},
					{"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"},
					true)
end
plugin_race_human()
