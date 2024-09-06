include (path.join(SCRIPTS_DIR, "utility.lua"))

function imgui()
	add_target_library("imgui",
					{},
					{"imgui_EXPORTS", "IMGUI_DISABLE_OBSOLETE_KEYIO"},
					{"nlohmann", "glm", "magic_enum", "taskflow", "Tracy"},
					{"core", "spdlog", "mimalloc", "hwinfo", "rttr"},
					false,
					"c++")
end
imgui()