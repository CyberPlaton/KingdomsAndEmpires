#include "sm_platform_raylib.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		void set_window_state(raylib::ConfigFlags flag, bool value)
		{
			value ? raylib::SetWindowState(flag) : raylib::ClearWindowState(flag);
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_raylib::init()
	{
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_raylib::shutdown()
	{
		const auto result = entry::renderer()->shutdown_device();

		raylib::ShutdownWindow();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_raylib::init_gfx(unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		if (entry::renderer()->init_device(nullptr, w, h, fullscreen, vsync) != opresult_ok)
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_raylib::init_mainwindow(stringview_t title, unsigned w, unsigned h,
		bool fullscreen)
	{
		//- configuration
		unsigned flags = window_flag_resizable |
			window_flag_decorated |
			window_flag_focus;

		if (fullscreen)
		{
			flags |= window_flag_fullscreen;
		}

		//- create the actual window
		raylib::InitWindow(w, h, title.data());

		set_window_state(raylib::FLAG_WINDOW_UNDECORATED, !algorithm::bit_on(flags, window_flag_decorated));
		set_window_state(raylib::FLAG_WINDOW_MINIMIZED, algorithm::bit_on(flags, window_flag_minimized));
		set_window_state(raylib::FLAG_WINDOW_RESIZABLE, algorithm::bit_on(flags, window_flag_resizable));
		set_window_state(raylib::FLAG_WINDOW_ALWAYS_RUN, algorithm::bit_on(flags, window_flag_run_minimized));
		set_window_state(raylib::FLAG_WINDOW_UNFOCUSED, !algorithm::bit_on(flags, window_flag_focus));

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_raylib::optional_init_event_mainloop()
	{
		//- empty
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_raylib::optional_process_event()
	{
		if (raylib::WindowShouldClose())
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplatform_raylib::main_window_position(unsigned* x, unsigned* y)
	{
		auto p = raylib::GetWindowPosition();

		*x = p.x;
		*y = p.x;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplatform_raylib::main_window_size(unsigned* x, unsigned* y)
	{
		*x = raylib::GetRenderWidth();
		*y = raylib::GetRenderHeight();
	}

} //- sm