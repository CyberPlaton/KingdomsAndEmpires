include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer()
	name = "spritemancer"
	build_options = {}
	define_flags = {"PLATFORM_DESKTOP"}
	additional_includes = {"src/detail/raylib_integration"}
	plugin_deps = {"plugin_logging"}
	plugin_headeronly_deps = {}
	thirdparty_deps = {"EASTL", "spdlog", "core", "bx"}
	thirdparty_headeronly_deps = {"nlohmann", "glm", "magic_enum", "taskflow", "Tracy", "argparse"}
	raylib_path = "src/detail/raylib_integration/raylib"
	glfw_path = path.join(raylib_path, "external", "glfw", "src")

	project(name)
		language (target_language)
		location (path.join(".project", name))
		targetname (name)

		kind ("StaticLib")

		-- raylib required files
		files{"src/**.h",
			  "src/**.cpp",
			  "src/**.hpp",
			  
			  path.join(raylib_path, "rcore.c"),
			  path.join(raylib_path, "rmodels.c"),
			  path.join(raylib_path, "rshapes.c"),
			  path.join(raylib_path, "rtext.c"),
			  path.join(raylib_path, "rtextures.c"),
			  path.join(raylib_path, "utils.c"),
			  
			  path.join(glfw_path, "internal.h"),
			  path.join(glfw_path, "platform.h"),
			  path.join(glfw_path, "mappings.h"),
			  path.join(glfw_path, "context.c"),
			  path.join(glfw_path, "init.c"),
			  path.join(glfw_path, "input.c"),
			  path.join(glfw_path, "monitor.c"),
			  path.join(glfw_path, "platform.c"),
			  path.join(glfw_path, "vulkan.c"),
			  path.join(glfw_path, "window.c"),
			  path.join(glfw_path, "egl_context.c"),
			  path.join(glfw_path, "osmesa_context.c"),
			  path.join(glfw_path, "null_platform.h"),
			  path.join(glfw_path, "null_joystick.h"),
			  path.join(glfw_path, "null_init.c"),
			  path.join(glfw_path, "null_monitor.c"),
			  path.join(glfw_path, "null_window.c"),
			  path.join(glfw_path, "null_joystick.c"),
			}

		-- glfw required files
		if PLATFORM == "windows" then
			defines{"_GLFW_WIN32"}
			files{path.join(glfw_path, "win32_time.c"),
				  path.join(glfw_path, "win32_thread.c"),
				  path.join(glfw_path, "win32_module.c"),
				  path.join(glfw_path, "win32_time.h"),
				  path.join(glfw_path, "win32_thread.h"),

				  path.join(glfw_path, "win32_platform.h"),
				  path.join(glfw_path, "win32_joystick.h"),
				  path.join(glfw_path, "win32_init.c"),
				  path.join(glfw_path, "win32_joystick.c"),
				  path.join(glfw_path, "win32_monitor.c"),
				  path.join(glfw_path, "win32_window.c"),
				  path.join(glfw_path, "wgl_context.c"),
			}
			links{"gdi32", "ws2_32", "kernel32", "opengl32", "psapi", "winmm"}
		elseif PLATFORM == "linux" then
			defines{"_GLFW_X11"}
			files{path.join(glfw_path, "posix_time.c"),
				  path.join(glfw_path, "posix_thread.c"),
				  path.join(glfw_path, "posix_module.c"),
				  path.join(glfw_path, "posix_time.h"),
				  path.join(glfw_path, "posix_thread.h"),

				  path.join(glfw_path, "x11_platform.h"),
				  path.join(glfw_path, "xkb_unicode.h"),
				  path.join(glfw_path, "x11_init.c"),
				  path.join(glfw_path, "x11_monitor.c"),
				  path.join(glfw_path, "x11_window.c"),
				  path.join(glfw_path, "xkb_unicode.c"),
				  path.join(glfw_path, "glx_context.c"),
				  path.join(glfw_path, "linux_joystick.h"),
				  path.join(glfw_path, "linux_joystick.c"),
				  path.join(glfw_path, "posix_poll.h"),
				  path.join(glfw_path, "posix_poll.c"),
			}
			links{"GL", "rt", "lm", "x11"}
		elseif PLATFORM == "macosx" then
			defines{"_GLFW_COCOA"}
			files{path.join(glfw_path, "cocoa_time.c"),
				  path.join(glfw_path, "posix_thread.c"),
				  path.join(glfw_path, "posix_module.c"),
				  path.join(glfw_path, "cocoa_time.h"),
				  path.join(glfw_path, "posix_thread.h"),

				  path.join(glfw_path, "cocoa_platform.h"),
				  path.join(glfw_path, "cocoa_joystick.h"),
				  path.join(glfw_path, "cocoa_init.m"),
				  path.join(glfw_path, "cocoa_joystick.m"),
				  path.join(glfw_path, "cocoa_monitor.m"),
				  path.join(glfw_path, "cocoa_window.m"),
				  path.join(glfw_path, "nsgl_context.m"),
			}
			links{"Cocoa", "IOKit", "CoreFoundation"}
		else
			print("Critical failure generating spritemancer project: 'Unknown Platform'!")
		end

		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		externalincludedirs {additional_includes}
		set_include_path_to_self(name)
		set_include_path_to_engine()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()
		set_bx_includes()

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
spritemancer()