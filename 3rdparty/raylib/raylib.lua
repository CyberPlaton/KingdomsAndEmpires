include (path.join(SCRIPTS_DIR, "utility.lua"))

function raylib()
	name = "raylib"
	build_options = {}
	define_flags = {"PLATFORM_DESKTOP", "BUILD_LIBTYPE_SHARED", "GLAD_API_CALL_EXPORT", "GLAD_API_CALL_EXPORT_BUILD"}
	additional_includes = {"src"}
	raylib_path = "src/raylib"
	glfw_path = path.join(raylib_path, "external", "glfw", "src")

	project(name)
		language ("c++")
		location (path.join(".project", name))
		targetname (name)

		kind ("SharedLib")

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
			links{"GL", "rt", "m", "X11"}
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
   
            -- Note: when using links on macosx premake creates a "-lLibrary", while for built-in
            -- libraries we require the "framework" keyword
            linkoptions{"-framework Cocoa -framework CoreVideo -framework IOKit -framework OpenGL -framework CoreFoundation"}
		else
			print("Critical failure generating spritemancer project: 'Unknown Platform'!")
		end

		externalincludedirs {additional_includes}
		includedirs {additional_includes}
		buildoptions{build_options}
		set_basic_defines()
		defines{define_flags}
		set_include_path_to_self(name)
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()
		configure()
end
raylib()
