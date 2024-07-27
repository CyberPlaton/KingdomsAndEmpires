include (path.join(SCRIPTS_DIR, "utility.lua"))

function core()
	add_target_static_library("core",
					{},
					{"core_EXPORTS"},
					{},
					{"spdlog", "mimalloc", "hwinfo", "rttr"},
					"c++",
					{}, -- plugin_headeronly_deps
					{"nlohmann", "glm", "magic_enum", "taskflow", "Tracy"}, -- thirdparty_headeronly_deps
					{"src"} -- additional_includes
					)
end
core()