include (path.join(SCRIPTS_DIR, "utility.lua"))

function imgui()
	add_target_library("imgui",
					{},
					{"imgui_EXPORTS"},
					{"nlohmann", "glm", "magic_enum", "taskflow"},
					{"core", "spdlog", "mimalloc", "hwinfo", "rttr"},
					false,
					"c++")
end
imgui()