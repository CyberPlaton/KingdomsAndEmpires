include (path.join(SCRIPTS_DIR, "utility.lua"))

function tiny_editor()

	build_options = {}
	define_flags = {}
	plugin_deps = {"plugin_module_example", "plugin_kingdoms_core", "plugin_camera_system", "plugin_race_human", "plugin_kingdoms_and_empires", "plugin_status_effects"}
	thirdparty_deps = {"Lua", "spdlog", "core", "flecs", "spritemancer", "mimalloc", "hwinfo", "rttr", "imgui", "raylib", "tree_sitter"}
	plugin_headeronly_deps = {}
	thirdparty_headeronly_deps = {"nlohmann", "glm", "magic_enum", "taskflow", "argparse", "LuaBridge", "Tracy", "pfd", "asio"}
	additional_includes = {"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include", "../../3rdparty/tree_sitter/tree_sitter/include"}
	name = "tiny_editor"

	-- Creating the tiny_editor is a special case, so we duplicate functions from add_target_project and add_project_main_app
	-- First we require the project for tiny_editor, moreover we require the project main application to include and link
	-- with current active project set from premake config
	group("projects/" .. name)

	-- Duplicate add_project_main_app
	app_name = name .. "_main"

	if VERBOSE == true then
		print("\tproject main application: " .. name .. " (".. app_name ..")")
	end

	project(app_name)
		location (path.join(".project", app_name))

		kind ("WindowedApp")
		files{"main.cpp",
			  "src/**.h",
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

		-- include active project, you cannot link to it, as it is an application itself,
		-- what we might want to do is link and inlcude deps of the active project recursively, but thats harder
		set_include_path(PROJECTS_DIR, ACTIVE_PROJECT)
		configure()
	group ""
end
tiny_editor()
