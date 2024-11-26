include (path.join(SCRIPTS_DIR, "utility.lua"))

function tracy()
	add_target_library("tracy",
					{},
					{},
					{},
					{},
					true,
					"c++",
					{"tracy/public"})
end
tracy()