include (path.join(SCRIPTS_DIR, "utility.lua"))

function core()
	add_target_static_library("core",
					{},
					{"core_EXPORTS"},
					{},
					{"rttr", "EASTL", "spdlog", "mimalloc"},
					"c++",
					{}, -- plugin_headeronly_deps
					{"rapidjson", "nlohmann", "glm", "magic_enum", "taskflow"}, -- thirdparty_headeronly_deps
					{} -- additional_includes
					)
end
core()