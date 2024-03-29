include (path.join(SCRIPTS_DIR, "utility.lua"))

function slang()
	add_target_static_library("slang",
					{},
					{"slang_EXPORTS"},
					{},
					{},
					"c++",
					{}, -- plugin_headeronly_deps
					{}, -- thirdparty_headeronly_deps
					{} -- additional_includes
					)
end
slang()