------------------------------------------------------------------------------------------------------------------------
function set_basic_defines()
	externalanglebrackets "On"

	if PLATFORM == "windows" then
		buildoptions{"/bigobj"}
		editandcontinue "Off"
		filter {"action:vs*"}
			buildoptions{"/Zc:__cplusplus"}
		filter{}
	elseif PLATFORM == "linux" then
	elseif PLATFORM == "macosx" then
	else
	end

	filter{"configurations:debug"}
		defines{"DEBUG=1", "TRACY_ENABLE", "BX_CONFIG_DEBUG"}
	filter{"configurations:release"}
		defines{"NDEBUG", "RELEASE=1", "TRACY_ENABLE", "BX_CONFIG_RELEASE"}
	filter{}
	defines{"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
			"_CRT_SECURE_NO_WARNINGS",
			"__STDC_FORMAT_MACROS",
			"_CRT_SECURE_NO_DEPRECATE"}
end

------------------------------------------------------------------------------------------------------------------------
function configure()
	filter{"configurations:debug"}
		symbols "On"
		optimize "Off"
	filter{"configurations:hybrid"}
		symbols "On"
		optimize "Full"
	filter{"configurations:release"}
		symbols "Off"
		optimize "Full"
	filter{}
end

------------------------------------------------------------------------------------------------------------------------
function load_project(project, dir)
	include(path.join(dir, project) .. ".lua")
end

------------------------------------------------------------------------------------------------------------------------
function set_include_path_to_engine()
	externalincludedirs {path.join(WORKSPACE_DIR, "engine", "include")}
end

------------------------------------------------------------------------------------------------------------------------
function link_with_engine()
	links{"engine"}
end

------------------------------------------------------------------------------------------------------------------------
function set_include_path_to_self(name)
	externalincludedirs {"include"}
	-- for some thirdparty libraries required, so we set just in case for all
	externalincludedirs {path.join("include", name)}

	includedirs {"include"}
	-- for some thirdparty libraries required, so we set just in case for all
	includedirs {path.join("include", name)}
end

------------------------------------------------------------------------------------------------------------------------
function set_include_path(is_third_party, name)

	if is_third_party == true then
		externalincludedirs {path.join(WORKSPACE_DIR, "3rdparty", name, "include")}
	else
		externalincludedirs {path.join(WORKSPACE_DIR, "plugins", name, "include")}
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
function set_bx_includes()
	externalincludedirs {path.join(WORKSPACE_DIR, "3rdparty", "bx", "bx", "include")}
	if PLATFORM == "windows" then
		externalincludedirs {path.join(WORKSPACE_DIR, "3rdparty", "bx", "bx", "include/compat/msvc")}
	elseif PLATFORM == "linux" then
		externalincludedirs {path.join(WORKSPACE_DIR, "3rdparty", "bx", "bx", "include/compat/linux")}
	elseif PLATFORM == "macosx" then
		externalincludedirs {path.join(WORKSPACE_DIR, "3rdparty", "bx", "bx", "include/compat/osx")}
	else
		print("Unknown platform!")
	end
end

------------------------------------------------------------------------------------------------------------------------
function set_sdl_deps()
	externalincludedirs {path.join(WORKSPACE_DIR, "3rdparty", "sdl", "include")}
	links{"SDL2", "SDL2main"}
end

------------------------------------------------------------------------------------------------------------------------
function set_libs_path()
	libdirs{path.join(VENDOR_DIR, OUTDIR)}
end

-- Creates a static library project. By default c sources are compiled too.
------------------------------------------------------------------------------------------------------------------------
function add_target_static_library(name, build_options, define_flags, plugin_deps, thirdparty_deps, target_language,
	plugin_headeronly_deps, thirdparty_headeronly_deps, additional_includes, requires_bx_includes)
	if VERBOSE == true then
		print("\tstatic library: " .. name)
	end
	project(name)
		language (target_language)
		location (path.join(".project", name))
		targetname (name)

		kind ("StaticLib")

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		externalincludedirs {additional_includes}
		set_include_path_to_self(name)
		set_include_path_to_engine()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		if requires_bx_includes == true then
			set_bx_includes()
			set_sdl_deps()
		end

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
		for ii = 1, #plugin_headeronly_deps do
			p = plugin_headeronly_deps[ii]
			set_include_path(false, p)
		end
		for ii = 1, #thirdparty_headeronly_deps do
			p = thirdparty_headeronly_deps[ii]
			set_include_path(true, p)
		end

		configure()
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

		configure()
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
		externalincludedirs {additional_includes}
		set_include_path_to_self(name)
		set_include_path_to_engine()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		--set_libs_path()

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

		configure()
end

-- special function for defining plugins that are linking with the engine
------------------------------------------------------------------------------------------------------------------------
function add_target_plugin(name, build_options, define_flags, plugin_deps, thirdparty_deps, plugin_headeronly_deps,
	thirdparty_headeronly_deps, additional_includes, depends_on_engine)
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
		externalincludedirs {additional_includes}
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
		for ii = 1, #plugin_headeronly_deps do
			p = plugin_headeronly_deps[ii]
			set_include_path(false, p)
		end
		for ii = 1, #thirdparty_headeronly_deps do
			p = thirdparty_headeronly_deps[ii]
			set_include_path(true, p)
		end

		-- link with engine by default, if not explicitly disabled
		if depends_on_engine == true then
			link_with_engine()
		end

		defines{name.. "_EXPORTS"}

		configure()
end

------------------------------------------------------------------------------------------------------------------------
function add_target_app(name, build_options, define_flags, thirdparty_deps, plugin_deps, plugin_headeronly_deps,
	thirdparty_headeronly_deps, additional_includes, requires_bx_includes)
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
		externalincludedirs {"include"}
		externalincludedirs {additional_includes}
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
		for ii = 1, #plugin_headeronly_deps do
			p = plugin_headeronly_deps[ii]
			set_include_path(false, p)
		end
		for ii = 1, #thirdparty_headeronly_deps do
			p = thirdparty_headeronly_deps[ii]
			set_include_path(true, p)
		end

		if PLATFORM == "windows" then
			links{"gdi32", "ws2_32", "kernel32", "opengl32", "psapi", "winmm"}
		elseif PLATFORM == "linux" then
		else
		end

		configure()
end
