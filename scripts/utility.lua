------------------------------------------------------------------------------------------------------------------------
function set_basic_defines()
	filter{"configurations:debug"}
		defines{"DEBUG=1"}
	filter{"configurations:release"}
		defines{"NDEBUG", "RELEASE=1"}
	filter{}
	defines{"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"}
end

------------------------------------------------------------------------------------------------------------------------
function load_project(project, dir)
	include(path.join(dir, project) .. ".lua")
end

------------------------------------------------------------------------------------------------------------------------
function set_include_path_to_engine()
	includedirs{path.join(WORKSPACE_DIR, "engine", "include")}
end

------------------------------------------------------------------------------------------------------------------------
function link_with_engine()
	links{"engine"}
end

function set_include_path_to_self(name)
	includedirs{"include"}
	-- for some thirdparty libraries required, so we set just in case for all
	includedirs{path.join("include", name)}
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
function add_target_static_library(name, build_options, define_flags, plugin_deps, thirdparty_deps, target_language,
	plugin_headeronly_deps, thirdparty_headeronly_deps, additional_includes)
	if VERBOSE == true then
		print("\tstatic library: " .. name)
	end
	project(name)
		language (target_language)
		location (path.join(".project", name))

		kind ("StaticLib")

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		includedirs{additional_includes}
		set_include_path_to_self(name)
		set_include_path_to_engine()
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

		-- set includes only from other plugins and thirdparty
		for ii = 1, #thirdparty_headeronly_deps do
			p = thirdparty_headeronly_deps[ii]
			set_include_path(true, p)
		end
		for ii = 1, #plugin_headeronly_deps do
			p = plugin_headeronly_deps[ii]
			set_include_path(false, p)
		end

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end

------------------------------------------------------------------------------------------------------------------------
function add_target_library(name, build_options, define_flags, plugin_deps, thirdparty_deps, headeronly, target_language)
	if VERBOSE == true then
		print("\tshared library: " .. name)
	end
	project(name)
		language (target_language)
		location (path.join(".project", name))

		if headeronly == true then
			kind ("None")
		else
			kind ("SharedLib")
		end

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		set_include_path_to_self(name)
		set_include_path_to_engine()
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

		-- set additional default defines
		defines{name .. "_EXPORTS"}

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end

------------------------------------------------------------------------------------------------------------------------
function add_target_library_ex(name, build_options, define_flags, plugin_deps, thirdparty_deps, headeronly, target_language, additional_includes)
	if VERBOSE == true then
		print("\tshared library: " .. name)
	end
	project(name)
		language (target_language)
		location (path.join(".project", name))

		if headeronly == true then
			kind ("None")
		else
			kind ("SharedLib")
		end

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		includedirs{additional_includes}
		set_include_path_to_self(name)
		set_include_path_to_engine()
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

		-- set additional default defines
		defines{name .. "_EXPORTS"}

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end

-- special function for defining plugins that are linking with the engine
------------------------------------------------------------------------------------------------------------------------
function add_target_plugin(name, build_options, define_flags, plugin_deps, thirdparty_deps, additional_includes, depends_on_engine)
	if VERBOSE == true then
		print("\tplugin: " .. name)
	end
	project(name)
		language ("c++")
		location (path.join(".project", name))

		kind ("StaticLib")

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		includedirs{additional_includes}
		set_include_path_to_self(name)
		set_include_path_to_engine()
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

		-- link with engine by default, if not explicitly disabled
		if depends_on_engine ~= nil and depends_on_engine == true then
			link_with_engine()
		end

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end

------------------------------------------------------------------------------------------------------------------------
function add_target_app(name, build_options, define_flags, thirdparty_deps, plugin_deps)
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
		set_basic_defines()
		defines{define_flags}
		includedirs{"include"}
		set_include_path_to_engine()
		link_with_engine()
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

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end