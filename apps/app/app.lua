include (path.join(SCRIPTS_DIR, "utility.lua"))

function app()
	add_target_app("app",
					{},
					{},
					{"test"},
					{"box2d"})
end
app()