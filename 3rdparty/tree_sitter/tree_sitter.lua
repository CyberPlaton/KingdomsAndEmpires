include (path.join(SCRIPTS_DIR, "utility.lua"))

function tree_sitter()
	project("tree_sitter")
		language ("c")
		location (path.join(".project", "tree_sitter"))

		kind ("StaticLib")

		files { "tree_sitter/lib/src/lib.c" }
		includedirs { "tree_sitter/lib/include", "tree_sitter/lib/src" }

		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))

		configure()
end
tree_sitter()