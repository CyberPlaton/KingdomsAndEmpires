include (path.join(SCRIPTS_DIR, "utility.lua"))

function sokol()
	project("sokol")
		language "c"
		location (path.join(".project", name))
		kind ("SharedLib")

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{""}
		defines{""}
		includedirs{"include"}
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		-- include and link deps from other plugins and thirdparty
		--for ii = 1, #plugin_deps do
		--	p = plugin_deps[ii]
		--	links{p}
		--	set_include_path(false, p)
		--end
		--for ii = 1, #thirdparty_deps do
		--	p = thirdparty_deps[ii]
		--	links{p}
		--	set_include_path(true, p)
		--end

		filter{"debug"}
			symbols "On"
			optimize "Off"

		filter{"release"}
			symbols "On"
			optimize "Full"
		filter{}

		-- set additional default defines
		defines{"sokol_EXPORTS"}
end
sokol()