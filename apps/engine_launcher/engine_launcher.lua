include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine_launcher()
	add_target_app("engine_launcher",
					{},
					{},
					{"test", "test2"},
					{"box2d", "sokol"})
end
engine_launcher()