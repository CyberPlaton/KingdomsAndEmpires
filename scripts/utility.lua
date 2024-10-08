IS_TRACY_ENABLED				= false
IS_MIMALLOC_ENABLED				= true
IS_PROFILE_ENABLED				= false
IS_LOGGING_ENABLED				= true
IS_WARNING_AND_ERRORS_ENABLED	= false

------------------------------------------------------------------------------------------------------------------------
function isempty(s)
	return s == nil or s == ''
end

------------------------------------------------------------------------------------------------------------------------
function set_and_disable_common_warnings_errors()
		filter {"action:vs*"}
			buildoptions{"/wd4005"}						-- disable "macro redefined" warning
		filter{"action:gmake*", "action:clang*"}
			buildoptions{"-Wno-builtin-macro-redefined"}
		filter{}
end

------------------------------------------------------------------------------------------------------------------------
function set_basic_defines()
	externalanglebrackets "On"

	if PLATFORM == "windows" then
		buildoptions{"/bigobj"}
		editandcontinue "Off"
		filter {"action:vs*"}
			buildoptions{"/Zc:__cplusplus", "/Zc:preprocessor"}
		filter{}
	elseif PLATFORM == "linux" then
	elseif PLATFORM == "macosx" then
	else
	end

	if IS_TRACY_ENABLED == true then
		defines{"TRACY_ENABLE"}
	end
	if IS_MIMALLOC_ENABLED == true then
		defines{"MIMALLOC_ENABLE"}
	end
	if IS_PROFILE_ENABLED == true then
		defines{"PROFILE_ENABLE"}
	end
	if IS_LOGGING_ENABLED == true then
		defines{"LOGGING_ENABLE"}
	end

	filter{"configurations:debug"}
		defines{"DEBUG=1"}
	filter{"configurations:release"}
		defines{"NDEBUG", "RELEASE=1"}
	filter{"configurations:hybrid"}
		defines{"NDEBUG", "HYBRID=1"}
	filter{}
	defines{"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
			"_CRT_SECURE_NO_WARNINGS",
			"__STDC_FORMAT_MACROS",
			"_CRT_SECURE_NO_DEPRECATE",
			"RTTR_DLL",						-- RTTR
			"BUILD_LIBTYPE_SHARED",			-- raylib
			"GLAD_API_CALL_EXPORT",			-- raylib glad/glad_es2
			}

	if IS_WARNING_AND_ERRORS_ENABLED == false then
		set_and_disable_common_warnings_errors()
	end
end

------------------------------------------------------------------------------------------------------------------------
function set_basic_links()
	if PLATFORM == "windows" then
		links{"gdi32", "ws2_32", "kernel32", "opengl32", "psapi", "winmm"}
	elseif PLATFORM == "linux" then
		links{"GL", "rt", "m", "X11"}
	elseif PLATFORM == "macosx" then
		links{"Cocoa", "IOKit", "CoreFoundation"}
	else
	end
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
function set_libs_path()
	libdirs{path.join(VENDOR_DIR, OUTDIR)}
end

-- Creates a static library project. By default c sources are compiled too.
------------------------------------------------------------------------------------------------------------------------
function add_target_static_library(name, build_options, define_flags, plugin_deps, thirdparty_deps, target_language,
	plugin_headeronly_deps, thirdparty_headeronly_deps, additional_includes)
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
		externalincludedirs{additional_includes}
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
function add_target_library(name, build_options, define_flags, thirdparty_headeronly_deps, thirdparty_deps, headeronly, target_language, additional_includes)
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
		includedirs{additional_includes}
		externalincludedirs{additional_includes}
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		-- include and link deps from other plugins and thirdparty
		for ii = 1, #thirdparty_headeronly_deps do
			p = thirdparty_headeronly_deps[ii]
			set_include_path(true, p)
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
		externalincludedirs{additional_includes}
		includedirs{additional_includes}
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

		kind ("SharedLib")

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
	thirdparty_headeronly_deps, additional_includes, app_kind)
	if VERBOSE == true then
		print("\tapplication: " .. name)
	end
	project(name)
		location (path.join(".project", name))

		if isempty(app_kind) then
			kind ("WindowedApp")
		else
			kind (app_kind)
		end

		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  "src/**.c"}

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		externalincludedirs{"include"}
		externalincludedirs{additional_includes}
		includedirs{additional_includes}
		set_include_path_to_engine()
		link_with_engine()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()
		set_basic_links()

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

		configure()
end
