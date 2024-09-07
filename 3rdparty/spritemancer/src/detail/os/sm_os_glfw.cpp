#include "sm_os_glfw.hpp"

namespace sm
{
	namespace
	{
		static GLFWcursor* S_CURSOR;
		static bx::RingBufferControl S_INPUT_CONTROL = { 256 };

		//------------------------------------------------------------------------------------------------------------------------
		inline static core::key glfw_key_to_core(int k)
		{
			switch (k)
			{
			case GLFW_KEY_ESCAPE:      return core::key_esc;
			case GLFW_KEY_ENTER:       return core::key_return;
			case GLFW_KEY_TAB:         return core::key_tab;
			case GLFW_KEY_SPACE:       return core::key_space;
			case GLFW_KEY_BACKSPACE:   return core::key_backspace;
			case GLFW_KEY_UP:          return core::key_up;
			case GLFW_KEY_DOWN:        return core::key_down;
			case GLFW_KEY_LEFT:        return core::key_left;
			case GLFW_KEY_RIGHT:       return core::key_right;
			case GLFW_KEY_INSERT:      return core::key_insert;
			case GLFW_KEY_DELETE:      return core::key_delete;
			case GLFW_KEY_HOME:        return core::key_home;
			case GLFW_KEY_END:         return core::key_end;
			case GLFW_KEY_PAGE_UP:     return core::key_page_up;
			case GLFW_KEY_PAGE_DOWN:   return core::key_page_down;
			case GLFW_KEY_PRINT_SCREEN:return core::key_print;
			case GLFW_KEY_KP_ADD:      return core::key_plus;
			case GLFW_KEY_KP_SUBTRACT: return core::key_minus;
			case GLFW_KEY_LEFT_BRACKET:return core::key_left_bracket;
			case GLFW_KEY_RIGHT_BRACKET:return core::key_right_bracket;
			case GLFW_KEY_SEMICOLON:   return core::key_semicolon;
			case GLFW_KEY_APOSTROPHE:  return core::key_quote;
			case GLFW_KEY_COMMA:       return core::key_comma;
			case GLFW_KEY_PERIOD:      return core::key_period;
			case GLFW_KEY_SLASH:       return core::key_slash;
			case GLFW_KEY_BACKSLASH:   return core::key_backslash;
			case GLFW_KEY_GRAVE_ACCENT:return core::key_tilde;
			case GLFW_KEY_F1:          return core::key_f1;
			case GLFW_KEY_F2:          return core::key_f2;
			case GLFW_KEY_F3:          return core::key_f3;
			case GLFW_KEY_F4:          return core::key_f4;
			case GLFW_KEY_F5:          return core::key_f5;
			case GLFW_KEY_F6:          return core::key_f6;
			case GLFW_KEY_F7:          return core::key_f7;
			case GLFW_KEY_F8:          return core::key_f8;
			case GLFW_KEY_F9:          return core::key_f9;
			case GLFW_KEY_F10:         return core::key_f10;
			case GLFW_KEY_F11:         return core::key_f11;
			case GLFW_KEY_F12:         return core::key_f12;
			case GLFW_KEY_KP_0:        return core::key_numpad0;
			case GLFW_KEY_KP_1:        return core::key_numpad1;
			case GLFW_KEY_KP_2:        return core::key_numpad2;
			case GLFW_KEY_KP_3:        return core::key_numpad3;
			case GLFW_KEY_KP_4:        return core::key_numpad4;
			case GLFW_KEY_KP_5:        return core::key_numpad5;
			case GLFW_KEY_KP_6:        return core::key_numpad6;
			case GLFW_KEY_KP_7:        return core::key_numpad7;
			case GLFW_KEY_KP_8:        return core::key_numpad8;
			case GLFW_KEY_KP_9:        return core::key_numpad9;
			case GLFW_KEY_0:           return core::key_0;
			case GLFW_KEY_1:           return core::key_1;
			case GLFW_KEY_2:           return core::key_2;
			case GLFW_KEY_3:           return core::key_3;
			case GLFW_KEY_4:           return core::key_4;
			case GLFW_KEY_5:           return core::key_5;
			case GLFW_KEY_6:           return core::key_6;
			case GLFW_KEY_7:           return core::key_7;
			case GLFW_KEY_8:           return core::key_8;
			case GLFW_KEY_9:           return core::key_9;
			case GLFW_KEY_A:           return core::key_a;
			case GLFW_KEY_B:           return core::key_b;
			case GLFW_KEY_C:           return core::key_c;
			case GLFW_KEY_D:           return core::key_d;
			case GLFW_KEY_E:           return core::key_e;
			case GLFW_KEY_F:           return core::key_f;
			case GLFW_KEY_G:           return core::key_g;
			case GLFW_KEY_H:           return core::key_h;
			case GLFW_KEY_I:           return core::key_i;
			case GLFW_KEY_J:           return core::key_j;
			case GLFW_KEY_K:           return core::key_k;
			case GLFW_KEY_L:           return core::key_l;
			case GLFW_KEY_M:           return core::key_m;
			case GLFW_KEY_N:           return core::key_n;
			case GLFW_KEY_O:           return core::key_o;
			case GLFW_KEY_P:           return core::key_p;
			case GLFW_KEY_Q:           return core::key_q;
			case GLFW_KEY_R:           return core::key_r;
			case GLFW_KEY_S:           return core::key_s;
			case GLFW_KEY_T:           return core::key_t;
			case GLFW_KEY_U:           return core::key_u;
			case GLFW_KEY_V:           return core::key_v;
			case GLFW_KEY_W:           return core::key_w;
			case GLFW_KEY_X:           return core::key_x;
			case GLFW_KEY_Y:           return core::key_y;
			case GLFW_KEY_Z:           return core::key_z;
			case GLFW_GAMEPAD_BUTTON_A:            return core::key_gamepad_a;
			case GLFW_GAMEPAD_BUTTON_B:            return core::key_gamepad_b;
			case GLFW_GAMEPAD_BUTTON_X:            return core::key_gamepad_x;
			case GLFW_GAMEPAD_BUTTON_Y:            return core::key_gamepad_y;
			case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:  return core::key_gamepad_shoulder_L;
			case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return core::key_gamepad_shoulder_R;
			case GLFW_GAMEPAD_BUTTON_BACK:         return core::key_gamepad_back;
			case GLFW_GAMEPAD_BUTTON_START:        return core::key_gamepad_start;
			case GLFW_GAMEPAD_BUTTON_GUIDE:        return core::key_gamepad_guide;
			case GLFW_GAMEPAD_BUTTON_DPAD_UP:      return core::key_gamepad_up;
			case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:    return core::key_gamepad_down;
			case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:    return core::key_gamepad_left;
			case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:   return core::key_gamepad_right;
			case GLFW_GAMEPAD_BUTTON_LEFT_THUMB:   return core::key_gamepad_thumb_L;
			case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB:  return core::key_gamepad_thumb_R;
			default: return core::key_none;
			}
		}

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
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::skey_button>(glfw_key_to_core(key), scancode, action, mods);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
		{
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::smouse_button>(button, action, mods);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_cursor_callback(GLFWwindow* window, double mx, double my)
		{
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::scursor>(mx, my);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_scroll_callback(GLFWwindow* window, double dx, double dy)
		{
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::smouse_scroll>(dx, dy);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void glfw_character_callback(GLFWwindow* window, unsigned codepoint)
		{
			core::cservice_manager::find<core::cevent_service>()->emit_event<events::window::scharacter_input>(codepoint);
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
	cos_glfw::cos_glfw() = default;

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
			glfwSetScrollCallback(m_mainwindow, glfw_scroll_callback);
			glfwSetCharCallback(m_mainwindow, glfw_character_callback);
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
		m_scroll_dt_x = 0.0;
		m_scroll_dt_y = 0.0;

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::on_event(const rttr::variant& event)
	{
		if (event.is_type<events::window::sresize>())
		{
			const auto& e = event.convert<events::window::sresize>();
			m_mainwindow_width = e.w;
			m_mainwindow_height = e.h;
		}
		else if (event.is_type<events::window::scursor>())
		{
			const auto& e = event.convert<events::window::scursor>();
			m_mouse.set_cursor(e.mx, e.my);
		}
		else if (event.is_type<events::window::smouse_button>())
		{
			const auto& e = event.convert<events::window::smouse_button>();
			//- Note: action specifies whether the key was pressed, released or is held, where
			//- mods specifies whether a modifier key was active at that time, i.e. GLFW_MOD_SHIFT
			const auto released = e.action == GLFW_RELEASE;
			const auto pressed = e.action == GLFW_PRESS;
			const auto held = e.action == GLFW_REPEAT;

			m_mouse.set_state((core::mouse_button)(e.button + 1), released, pressed, held);
		}
		else if (event.is_type<events::window::skey_button>())
		{
			const auto& e = event.convert<events::window::skey_button>();
			const auto released = e.action == GLFW_RELEASE;
			const auto pressed = e.action == GLFW_PRESS;
			const auto held = e.action == GLFW_REPEAT;

			m_keyboard.set_state((core::key)e.button, released, pressed, held, e.mods);
		}
		else if (event.is_type<events::window::sminimize>())
		{
			const auto& e = event.convert<events::window::sminimize>();
		}
		else if (event.is_type<events::window::sunminimize>())
		{
			const auto& e = event.convert<events::window::sunminimize>();
		}
		else if (event.is_type<events::window::shide>())
		{
			const auto& e = event.convert<events::window::shide>();
		}
		else if (event.is_type<events::window::sunhide>())
		{
			const auto& e = event.convert<events::window::sunhide>();
		}
		else if (event.is_type<events::window::sfocus>())
		{
			const auto& e = event.convert<events::window::sfocus>();
		}
		else if (event.is_type<events::window::sunfocus>())
		{
			const auto& e = event.convert<events::window::sunfocus>();
		}
		else if (event.is_type<events::window::smouse_scroll>())
		{
			const auto& e = event.convert<events::window::smouse_scroll>();

			m_mouse.m_scroll_x += e.dx;
			m_mouse.m_scroll_y += e.dy;

			m_scroll_dt_x = m_mouse.m_scroll_x - m_previous_mouse_scroll_x;
			m_scroll_dt_y = m_mouse.m_scroll_y - m_previous_mouse_scroll_y;

			m_previous_mouse_scroll_x = m_mouse.m_scroll_x;
			m_previous_mouse_scroll_y = m_mouse.m_scroll_y;
		}
		else if (event.is_type<events::window::scharacter_input>())
		{
			const auto& e = event.convert<events::window::scharacter_input>();

			byte_t chars[4];
			if (byte_t length = algorithm::encode_utf8(chars, e.codepoint); length > 0)
			{
				for (unsigned l = S_INPUT_CONTROL.reserve(4); l < length; l = S_INPUT_CONTROL.reserve(4))
				{
					if (S_INPUT_CONTROL.available() > 0)
					{
						S_INPUT_CONTROL.consume(4);
					}
				}

				bx::memCopy(&m_input_chars[S_INPUT_CONTROL.m_current], chars, 4);
				S_INPUT_CONTROL.commit(4);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cos_glfw::read_input_character()
	{
		if (S_INPUT_CONTROL.available() > 0)
		{
			const auto* utf8 = &m_input_chars[S_INPUT_CONTROL.m_read];
			S_INPUT_CONTROL.consume(4);
			return *utf8;
		}
		return (unsigned)'\0';
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
	void cos_glfw::mouse_scroll_dt(double* x, double* y)
	{
		*x = m_scroll_dt_x;
		*y = m_scroll_dt_y;
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