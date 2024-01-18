include (path.join(SCRIPTS_DIR, "utility.lua"))

function engine()
	add_target_static_library("engine",
		{},
		{},
		{},
		{"box2d", "sokol"},
		"c++",
		{},
		{"test", "test2"})
end
engine()