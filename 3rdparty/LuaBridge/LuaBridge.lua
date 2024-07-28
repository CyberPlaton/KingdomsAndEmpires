include (path.join(SCRIPTS_DIR, "utility.lua"))

function LuaBridge()
	add_target_library("LuaBridge",
					{},
					{},
					{},
					{"Lua"},
					true,
					"c++")
end
LuaBridge()