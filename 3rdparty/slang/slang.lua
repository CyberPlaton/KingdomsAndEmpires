include (path.join(SCRIPTS_DIR, "utility.lua"))

function slang()
	add_target_library("slang",
					{},
					{"slang_EXPORTS"},
					{},
					{"rttr"},
					false,
					"c++",
					{"../rttr/rttr/src"})
end
slang()