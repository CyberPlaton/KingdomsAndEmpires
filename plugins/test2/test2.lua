include (path.join(SCRIPTS_DIR, "utility.lua"))

function test2()
	add_target_library("test2",
					{},
					{},
					{},
					{})
end
test2()