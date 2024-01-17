include (path.join(SCRIPTS_DIR, "utility.lua"))

function box2d()
	add_target_library("box2d",
					{},
					{},
					{},
					{},
					false)
end
box2d()