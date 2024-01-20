include (path.join(SCRIPTS_DIR, "utility.lua"))

function taskflow()
	add_target_library("taskflow",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
taskflow()