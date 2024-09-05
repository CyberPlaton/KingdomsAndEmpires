#include "sm_os_glfw.hpp"

namespace sm
{
	namespace
	{
		static double S_CURSOR_X = 0.0;
		static double S_CURSOR_Y = 0.0;
		static int S_MOUSE_BUTTON = 0;
		static int S_MOUSE_ACTION = 0;
		static int S_MOUSE_MODS = 0;
		static int S_KEY_KEY = 0;
		static int S_KEY_SCANCODE = 0;
		static int S_KEY_ACTION = 0;
		static int S_KEY_MODS = 0;
		static int S_W = 0;
		static int S_H = 0;

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

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_window_size_callback(GLFWwindow* window, int width, int height)
		{
			S_W = width;
			S_H = height;

			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::sresize>(S_W, S_H);
		}

		//- Redirecting glfw key strokes to main application
		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			S_KEY_KEY = key;
			S_KEY_SCANCODE = scancode;
			S_KEY_ACTION = action;
			S_KEY_MODS = mods;

			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::skey_button>(S_KEY_KEY, S_KEY_SCANCODE, S_KEY_ACTION, S_KEY_MODS);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_mouse_button_callback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
		{
			S_MOUSE_BUTTON = button;
			S_MOUSE_ACTION = action;
			S_MOUSE_MODS = mods;

			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::smouse_button>(S_MOUSE_BUTTON, S_MOUSE_ACTION, S_MOUSE_MODS);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_cursor_callback(GLFWwindow* _window, double mx, double my)
		{
			S_CURSOR_X = mx;
			S_CURSOR_Y = my;

			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::scursor>(S_CURSOR_X, S_CURSOR_Y);
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
		entry::get_renderer()->prepare_device();

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
			glfwSetMouseButtonCallback(m_mainwindow, glfw_mouse_button_callback);
			glfwSetCursorPosCallback(m_mainwindow, glfw_cursor_callback);
			glfwSetWindowSizeCallback(m_mainwindow, glfw_window_size_callback);
			S_W = w;
			S_H = h;

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
		*x = S_W;
		*y = S_H;
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
	void cos_glfw::mouse_position(double* x, double* y)
	{
		*x = S_CURSOR_X;
		*y = S_CURSOR_Y;
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