#include "sm_os_glfw.hpp"

namespace sm
{
	namespace
	{
		//- Redirecting error from glfw to reporter
		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_error_callback(int error, const char* description)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback((core::logging_verbosity)error,
					fmt::format("GLFW error '{}'", description));
			}
		}

		//- Redirecting glfw key strokes to main application
		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{

		}

		//- Retrieve native window handle from GLFWindow
		//------------------------------------------------------------------------------------------------------------------------
		inline static void* glfw_native_handle(GLFWwindow* window)
		{
#if CORE_PLATFORM_WINDOWS
			return glfwGetWin32Window(window);
#elif CORE_PLATFORM_LINUX | BX_PLATFORM_BSD
			return (void*)(uintptr_t)glfwGetX11Window(window);
#elif CORE_PLATFORM_OSX
			return glfwGetCocoaWindow(window);
#endif
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::init()
	{
		glfwSetErrorCallback(glfw_error_callback);

		if (glfwInit())
		{
			return opresult_ok;
		}
		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::shutdown()
	{
		glfwTerminate();
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::init_gfx(int w, int h, bool fullscreen, bool vsync)
	{
		if (entry::get_renderer()->init_device(glfw_native_handle(m_mainwindow), w, h, fullscreen, vsync) != opresult_ok)
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::init_mainwindow(stringview_t title, int w, int h, bool fullscreen)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		if (m_mainwindow = glfwCreateWindow(w, h, title.data(), nullptr, nullptr); m_mainwindow)
		{
			glfwSetKeyCallback(m_mainwindow, glfw_key_callback);

			return opresult_ok;
		}
		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::optional_init_event_mainloop()
	{
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::optional_process_event()
	{
		glfwPollEvents();
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::main_window_position(int* x, int* y)
	{
		glfwGetWindowPos(m_mainwindow, x, y);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::main_window_size(int* x, int* y)
	{
		glfwGetWindowSize(m_mainwindow, x, y);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_key_held(core::key k)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_key_pressed(core::key k)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_key_released(core::key k)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_mouse_button_held(core::mouse_button b)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_mouse_button_pressed(core::mouse_button b)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_mouse_button_released(core::mouse_button b)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float cos_glfw::gamepad_axis(core::gamepad_axis a)
	{
		return 0.0f;
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	rttr::cregistrator<cos_glfw>("cos_glfw")
		;
}