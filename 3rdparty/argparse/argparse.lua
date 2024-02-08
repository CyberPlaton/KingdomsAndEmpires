include (path.join(SCRIPTS_DIR, "utility.lua"))

function argparse()
	add_target_library("argparse",
					{},
					{},
					{},
					{},
					true,
					"c++")
end
argparse()