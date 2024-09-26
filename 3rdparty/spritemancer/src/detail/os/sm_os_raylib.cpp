#include "sm_os_raylib.hpp"

namespace sm
{
	namespace
	{
		static int S_WINDOW_STATE_FLAGS = 0;

		//------------------------------------------------------------------------------------------------------------------------
		void set_window_state(raylib::ConfigFlags flag, bool value)
		{
			value ? raylib::SetWindowState(flag) : raylib::ClearWindowState(flag);
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_raylib::init()
	{
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_raylib::shutdown()
	{
		const auto result = entry::get_renderer()->shutdown_device();

		raylib::ShutdownWindow();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_raylib::init_gfx(unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		if (entry::get_renderer()->init_device(nullptr, w, h, fullscreen, vsync) != opresult_ok)
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_raylib::init_mainwindow(stringview_t title, unsigned w, unsigned h,
		bool fullscreen)
	{
		//- configuration
		int flags = window_flag_resizable |
			window_flag_decorated |
			window_flag_focus;

		if (fullscreen)
		{
			flags |= window_flag_fullscreen;
		}

		//- create the actual window
		raylib::InitWindow(w, h, title.data());

		set_window_state(raylib::FLAG_WINDOW_UNDECORATED, !algorithm::bit_check(flags, window_flag_decorated));
		set_window_state(raylib::FLAG_WINDOW_MINIMIZED, algorithm::bit_check(flags, window_flag_minimized));
		set_window_state(raylib::FLAG_WINDOW_RESIZABLE, algorithm::bit_check(flags, window_flag_resizable));
		set_window_state(raylib::FLAG_WINDOW_ALWAYS_RUN, algorithm::bit_check(flags, window_flag_run_minimized));
		set_window_state(raylib::FLAG_WINDOW_FOCUSED, algorithm::bit_check(flags, window_flag_focus));

		//- cache current state of the window at start
		S_WINDOW_STATE_FLAGS = raylib::GetWindowState();

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_raylib::optional_init_event_mainloop()
	{
		//- empty
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_raylib::optional_process_event()
	{
		if (raylib::WindowShouldClose())
		{
			return opresult_fail;
		}

		//- intercept window related events
		if (auto* es = core::cservice_manager::find<core::cevent_service>(); es)
		{
			if (raylib::IsWindowResized())
			{
				es->emit_event<events::window::sresize>(raylib::GetRenderWidth(), raylib::GetRenderHeight());

				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_trace, "Window resized event");
				}
			}
			const auto minimized = raylib::IsWindowMinimized();
			const auto hidden = raylib::IsWindowHidden();
			const auto focused = raylib::IsWindowFocused();

			if (minimized && !algorithm::bit_check(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_MINIMIZED))
			{
				//- enter minimized state
				algorithm::bit_set(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_MINIMIZED);

				es->emit_event<events::window::sminimize>();

				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_trace, "Window minimize event");
				}
			}
			//- return from minimized state
			else if (!minimized && algorithm::bit_check(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_MINIMIZED))
			{
				algorithm::bit_clear(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_MINIMIZED);

				es->emit_event<events::window::sunminimize>();

				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_trace, "Window unminimize event");
				}
			}
			//- enter window hidden state
			if (hidden && !algorithm::bit_check(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_HIDDEN))
			{
				algorithm::bit_set(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_HIDDEN);

				es->emit_event<events::window::shide>();

				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_trace, "Window hide event");
				}
			}
			//- return from hidden window state
			else if (!hidden && algorithm::bit_check(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_HIDDEN))
			{
				algorithm::bit_clear(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_HIDDEN);

				es->emit_event<events::window::sunhide>();

				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_trace, "Window unhide event");
				}
			}
			//- enter window focused state
			if (focused && !algorithm::bit_check(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_FOCUSED))
			{
				algorithm::bit_set(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_FOCUSED);

				es->emit_event<events::window::sfocus>();

				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_trace, "Window focused event");
				}
			}
			//- enter window unfocused state
			else if (!focused && algorithm::bit_check(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_FOCUSED))
			{
				algorithm::bit_clear(S_WINDOW_STATE_FLAGS, raylib::FLAG_WINDOW_FOCUSED);

				es->emit_event<events::window::sunfocus>();

				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_trace, "Window unfocused event");
				}
			}
		}

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_raylib::main_window_position(unsigned* x, unsigned* y)
	{
		auto p = raylib::GetWindowPosition();

		*x = p.x;
		*y = p.x;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_raylib::main_window_size(unsigned* x, unsigned* y)
	{
		*x = raylib::GetRenderWidth();
		*y = raylib::GetRenderHeight();
	}

	//------------------------------------------------------------------------------------------------------------------------
	float cos_raylib::frametime() const
	{
		return raylib::GetFrameTime();
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	rttr::cregistrator<cos_raylib>("cos_raylib")
		;
}