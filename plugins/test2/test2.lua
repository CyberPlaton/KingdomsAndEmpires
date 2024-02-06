include (path.join(SCRIPTS_DIR, "utility.lua"))

function test2()
	add_target_plugin("test2",
					{},
					{},
					{},
					{},
					{},
					false)
end
test2()