include (path.join(SCRIPTS_DIR, "utility.lua"))

function bgfx()
	project("bgfx")
		language ("c++")
		cppdialect "c++17"
		location (path.join(".project", "bgfx"))

		kind ("StaticLib")

		if PLATFORM == "windows" or PLATFORM == "linux" then
			files{path.join("bgfx", "src", "amalgamated.cpp")}
		elseif PLATFORM == "macosx" then
			files{path.join("bgfx", "src", "amalgamated.mm")}
		else
		end

		includedirs{"bgfx/3rdparty",
					"bgfx/include",
					"bgfx/3rdparty/cgltf",
					"bgfx/3rdparty/dear-imgui",
					"bgfx/3rdparty/directx-headers/include/directx",
					"bgfx/3rdparty/directx-headers/include/wsl",
					"bgfx/3rdparty/fcpp",
					"bgfx/3rdparty/glslang",
					"bgfx/3rdparty/glsl-optimizer",
					"bgfx/3rdparty/glsl-optimizer/include",
					"bgfx/3rdparty/iconfontheaders",
					"bgfx/3rdparty/khronos",
					"bgfx/3rdparty/meshoptimizer",
					"bgfx/3rdparty/native_app_glue",
					"bgfx/3rdparty/renderdoc",
					"bgfx/3rdparty/sdf",
					"bgfx/3rdparty/spirv-cross",
					"bgfx/3rdparty/spirv-cross/include",
					"bgfx/3rdparty/spirv-headers/include",
					"bgfx/3rdparty/spirv-tools/include",
					"bgfx/3rdparty/stv",
					path.join(WORKSPACE_DIR, "3rdparty", "bimg", "bimg", "include")}

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
bgfx()