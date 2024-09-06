#include "sm_os_glfw.hpp"

namespace sm
{
	namespace
	{
		static GLFWcursor* S_CURSOR;

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
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::sresize>(width, height);
		}

		//- Redirecting glfw key strokes to main application
		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::skey_button>(key, scancode, action, mods);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
		{
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::smouse_button>(button, action, mods);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_cursor_callback(GLFWwindow* _window, double mx, double my)
		{
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::scursor>(mx, my);
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
			S_CURSOR = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

			glfwSetCursor(m_mainwindow, S_CURSOR);

			glfwSetKeyCallback(m_mainwindow, glfw_key_callback);
			glfwSetMouseButtonCallback(m_mainwindow, glfw_mouse_button_callback);
			glfwSetCursorPosCallback(m_mainwindow, glfw_cursor_callback);
			glfwSetWindowSizeCallback(m_mainwindow, glfw_window_size_callback);
			m_mainwindow_width = w;
			m_mainwindow_height = h;

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
	void cos_glfw::on_window_resize_event(int width, int height)
	{
		m_mainwindow_width = width;
		m_mainwindow_height = height;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::on_key_event(int key, int scancode, int action, int mods)
	{
		const auto released = action == GLFW_RELEASE;
		const auto pressed = action == GLFW_PRESS;
		const auto held = action == GLFW_REPEAT;

		m_keyboard.set_state((core::key)key, released, pressed, held, mods);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::on_mouse_button_event(int button, int action, int mods)
	{
		//- Note: action specifies whether the key was pressed, released or is held, where
		//- mods specifies whether a modifier key was active at that time, i.e. GLFW_MOD_SHIFT
		const auto released = action == GLFW_RELEASE;
		const auto pressed = action == GLFW_PRESS;
		const auto held = action == GLFW_REPEAT;

		m_mouse.set_state((core::mouse_button)(button + 1), released, pressed, held);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::on_cursor_event(double mx, double my)
	{
		m_mouse.set_cursor(mx, my);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::main_window_position(int* x, int* y)
	{
		glfwGetWindowPos(m_mainwindow, x, y);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::main_window_size(int* x, int* y)
	{
		*x = m_mainwindow_width;
		*y = m_mainwindow_height;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_key_held(core::key k) const
	{
		return m_keyboard.is_held(k);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_key_pressed(core::key k) const
	{
		return m_keyboard.is_pressed(k);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_key_released(core::key k) const
	{
		return m_keyboard.is_released(k);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_modifier_active(int modifiers) const
	{
		return m_keyboard.check_modifiers(modifiers);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::mouse_position(double* x, double* y)
	{
		*x = m_mouse.m_x;
		*y = m_mouse.m_y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_mouse_button_held(core::mouse_button b)
	{
		return m_mouse.is_held(b);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_mouse_button_pressed(core::mouse_button b)
	{
		return m_mouse.is_pressed(b);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cos_glfw::is_mouse_button_released(core::mouse_button b)
	{
		return m_mouse.is_released(b);
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