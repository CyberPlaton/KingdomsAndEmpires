include (path.join(SCRIPTS_DIR, "utility.lua"))

function bimg()
	project("bimg")
		language ("c++")
		cppdialect "c++17"
		location (path.join(".project", "bimg"))

		kind ("StaticLib")

		files{"bimg/src/*.cpp"}
		includedirs{"bimg/include",
					"bimg/3rdparty",
					"bimg/3rdparty/astc-encoder/include",
					"bimg/3rdparty/edtaa3",
					"bimg/3rdparty/etc1",
					"bimg/3rdparty/etc2",
					"bimg/3rdparty/iqa/include",
					"bimg/3rdparty/libsquish",
					"bimg/3rdparty/lodepng",
					"bimg/3rdparty/nvtt",
					"bimg/3rdparty/pvrtc",
					"bimg/3rdparty/stb",
					"bimg/3rdparty/tinyexr/deps/miniz",
					"bimg/3rdparty/tinyexr"}

		set_bx_includes()

		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()
		set_basic_defines()

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end
bimg()