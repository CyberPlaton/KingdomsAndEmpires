include (path.join(SCRIPTS_DIR, "utility.lua"))

function app()
	add_target_app("app",
					{},
					{},
					{},
					{"box2d", "sokol"})
end
app()