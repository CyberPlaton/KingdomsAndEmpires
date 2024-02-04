include (path.join(SCRIPTS_DIR, "utility.lua"))

function EASTL()
	project("EASTL")
		language ("c++")
		cppdialect "c++14"
		location (path.join(".project", "EASTL"))

		kind ("StaticLib")

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		defines{"_CHAR16T", "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS", "EASTL_OPENSOURCE=1"}
		includedirs{"include"}
		includedirs{path.join("include", "EABase")}
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
EASTL()