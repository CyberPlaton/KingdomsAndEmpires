include (path.join(SCRIPTS_DIR, "utility.lua"))

function mimalloc()
	project("mimalloc")
		language ("c")
		location (path.join(".project", "mimalloc"))

		kind ("StaticLib")

		files{"src/alloc.c",
			  "src/alloc-aligned.c",
			  "src/alloc-posix.c",
			  "src/arena.c",
			  "src/bitmap.c",
			  "src/heap.c",
			  "src/init.c",
			  "src/options.c",
			  "src/os.c",
			  "src/page.c",
			  "src/random.c",
			  "src/segment.c",
			  "src/segment-map.c",
			  "src/stats.c",
			  "src/prim/prim.c"}

		buildoptions{build_options}
		includedirs{"include"}
		set_include_path_to_engine()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end
mimalloc()