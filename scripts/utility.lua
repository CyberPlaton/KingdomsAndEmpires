------------------------------------------------------------------------------------------------------------------------
function load_project(project, dir)
	include(path.join(dir, project) .. ".lua")
end

------------------------------------------------------------------------------------------------------------------------
function set_include_path(is_third_party, name)

	if is_third_party == true then
		includedirs{path.join(WORKSPACE_DIR, "3rdparty", name, "include")}
	else
		includedirs{path.join(WORKSPACE_DIR, "plugins", name, "include")}
	end

	if VERBOSE == true then
		if is_third_party == true then
			print("\t\tadding dependency: " .. path.join(WORKSPACE_DIR, "3rdparty", name, "include"))
		else
			print("\t\tadding dependency: " .. path.join(WORKSPACE_DIR, "plugins", name, "include"))
		end
	end
end

------------------------------------------------------------------------------------------------------------------------
function set_libs_path()
	libdirs{path.join(VENDOR_DIR, OUTDIR)}
end

------------------------------------------------------------------------------------------------------------------------
function add_target_library(name, build_options, define_flags, plugin_deps, thirdparty_deps)
	if VERBOSE == true then
		print("\tshared library: " .. name)
	end
	project(name)
		location (path.join(".project", name))
		kind ("SharedLib")

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{build_options}
		defines{define_flags}
		includedirs{"include"}
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		-- include and link deps from other plugins and thirdparty
		for ii = 1, #plugin_deps do
			p = plugin_deps[ii]
			links{p}
			set_include_path(false, p)
		end
		for ii = 1, #thirdparty_deps do
			p = thirdparty_deps[ii]
			links{p}
			set_include_path(true, p)
		end

		filter{"debug"}
			symbols "On"
			optimize "Off"

		filter{"release"}
			symbols "On"
			optimize "Full"
		filter{}

		-- set additional default defines
		defines{name .. "_EXPORTS"}
end

------------------------------------------------------------------------------------------------------------------------
function add_target_app(name, build_options, define_flags, plugin_deps, thirdparty_deps)
	if VERBOSE == true then
		print("\tapplication: " .. name)
	end
	project(name)
		location (path.join(".project", name))
		kind ("WindowedApp")

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{build_options}
		defines{define_flags}
		includedirs{"include"}
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		-- include and link deps from other plugins and thirdparty
		for ii = 1, #plugin_deps do
			p = plugin_deps[ii]
			links{p}
			set_include_path(false, p)
		end
		for ii = 1, #thirdparty_deps do
			p = thirdparty_deps[ii]
			links{p}
			set_include_path(true, p)
		end

		filter{"debug"}
			symbols "On"
			optimize "Off"

		filter{"release"}
			symbols "On"
			optimize "Full"
		filter{}
end