include (path.join(SCRIPTS_DIR, "utility.lua"))

function plugin_rttr_example()
	name = "plugin_rttr_example"
	build_options = {}
	define_flags = {"RTTR_DLL"}
	additional_includes = {"../../3rdparty/rttr/rttr/src", "../../3rdparty/asio/include/asio/asio/include"}
	plugin_deps = {}
	plugin_headeronly_deps = {}
	thirdparty_deps = {"spdlog", "core", "mimalloc", "hwinfo", "rttr", "imgui"}
	thirdparty_headeronly_deps = {"nlohmann", "glm", "magic_enum", "taskflow", "Tracy", "argparse", "asio"}
	depends_on_engine = true

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
plugin_rttr_example()
