include (path.join(SCRIPTS_DIR, "utility.lua"))

function tiny_editor()

	build_options = {}
	define_flags = {}
	plugin_deps = {"plugin_module_example", "plugin_kingdoms_core", "plugin_camera_system", "plugin_race_human", "plugin_kingdoms_and_empires", "plugin_status_effects"}
	thirdparty_deps = {"Lua", "spdlog", "core", "flecs", "spritemancer", "mimalloc", "hwinfo", "rttr", "imgui", "bimg", "bx", "bgfx", "glfw"}
	plugin_headeronly_deps = {}
	thirdparty_headeronly_deps = {"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "pfd", "asio"}
	additional_includes = {"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"}
	name = "tiny_editor"

	-- Creating the tiny_editor is a special case, so we duplicate functions from add_target_project and add_project_main_app
	-- First we require the project for tiny_editor, moreover we require the project main application to include and link
	-- with current active project set from premake config
	group name
	add_target_static_library(name,
					build_options,
					define_flags,
					plugin_deps,
					thirdparty_deps,
					"C++",
					plugin_headeronly_deps
					thirdparty_headeronly_deps,
					additional_includes,
					true)

	-- Duplicate add_project_main_app
	app_name = name .. "_main"

	if VERBOSE == true then
		print("\tproject main application: " .. name .. " (".. app_name ..")")
	end

	project(app_name)
		location (path.join(".project", app_name))

		kind ("WindowedApp")
		files{"main.cpp"}

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		externalincludedirs{"include"}
		externalincludedirs{additional_includes}
		includedirs{additional_includes}
		set_include_path_to_engine()
		link_with_engine()
		set_bx_includes()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()
		set_basic_links()

		-- include and link deps from other plugins and thirdparty
		for ii = 1, #plugin_deps do
			p = plugin_deps[ii]
			links{p}
			set_include_path(PLUGINS_DIR, p)
		end
		for ii = 1, #thirdparty_deps do
			p = thirdparty_deps[ii]
			links{p}
			set_include_path(THIRDPARTY_DIR, p)
		end
		for ii = 1, #plugin_headeronly_deps do
			p = plugin_headeronly_deps[ii]
			set_include_path(PLUGINS_DIR, p)
		end
		for ii = 1, #thirdparty_headeronly_deps do
			p = thirdparty_headeronly_deps[ii]
			set_include_path(THIRDPARTY_DIR, p)
		end

		-- include and link the tiny_editor projects static library
		set_include_path(PROJECTS_DIR, name)
		links{name}

		-- include and link current active project
		set_include_path(PROJECTS_DIR, ACTIVE_PROJECT)
		links{ACTIVE_PROJECT}

		configure()
	group ""
end
tiny_editor()
