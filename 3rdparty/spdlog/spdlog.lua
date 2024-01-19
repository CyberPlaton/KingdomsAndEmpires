include (path.join(SCRIPTS_DIR, "utility.lua"))

function spdlog()
	add_target_library("spdlog",
					{},
					{"SPDLOG_COMPILED_LIB", "SPDLOG_SHARED_LIB"},
					{},
					{},
					false,
					"c++")
end
spdlog()