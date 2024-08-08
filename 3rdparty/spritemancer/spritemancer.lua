include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer()
	add_target_static_library("spritemancer",
					{},
					{"spritemancer_EXPORTS"},
					{},
					{"spdlog", "core", "mimalloc", "hwinfo", "rttr", "imgui", "raylib"},
					"c++",
					{}, -- plugin_headeronly_deps
					{"nlohmann", "glm", "magic_enum", "taskflow", "Tracy", "argparse"}, -- thirdparty_headeronly_deps
					{"src", "../rttr/rttr/src"} -- additional_includes
					)
end
spritemancer()