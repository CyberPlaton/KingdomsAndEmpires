include (path.join(SCRIPTS_DIR, "utility.lua"))

function test()
	add_target_library("test",
					{},
					{},
					{},
					{})
end
test()