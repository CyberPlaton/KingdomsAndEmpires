include (path.join(SCRIPTS_DIR, "utility.lua"))

function pfd()
	add_target_library("pfd",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
pfd()