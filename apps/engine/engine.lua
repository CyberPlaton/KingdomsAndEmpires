include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_app("engine",
					{},
					{},
					{},
					{"box2d", "sokol"})
end
engine()