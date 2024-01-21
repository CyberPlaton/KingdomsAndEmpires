include (path.join(SCRIPTS_DIR, "utility.lua"))

function flecs()
	add_target_library("flecs",
					{},
					{"flecs_EXPORTS"},
					{},
					{},
					false,
					"c")
end
flecs()