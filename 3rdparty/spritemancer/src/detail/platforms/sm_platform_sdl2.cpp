#include "sm_platform_sdl2.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		void* native_window_handle(SDL_Window* window)
		{
			SDL_SysWMinfo info;
			SDL_VERSION(&info.version);

			SDL_GetWindowWMInfo(window, &info);

#if CORE_PLATFORM_WINDOWS
			return (void*)info.info.win.window;
#elif CORE_PLATFORM_LINUX
			return (void*)info.info.x11.window;
#elif CORE_PLATFORM_OSX
			return (void*)info.info.cocoa.window;
#else
			return nullptr;
#endif
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::init()
	{
		SDL_Init(SDL_INIT_VIDEO);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::shutdown()
	{
		SDL_DestroyWindow(S_WINDOW);
		SDL_Quit();
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::init_on_thread()
	{
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::shutdown_on_thread()
	{
		return entry::renderer()->shutdown_device();
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::init_gfx(unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		if (entry::renderer()->init_device(native_window_handle(S_WINDOW), w, h, fullscreen, vsync) != opresult_ok)
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::init_mainwindow(stringview_t title, unsigned w, unsigned h,
		bool fullscreen)
	{
		unsigned flags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

		if (fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		S_WINDOW = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::optional_init_event_mainloop()
	{
		//- empty
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::process_event()
	{
		SDL_Event e;

		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_APP_TERMINATING:
			case SDL_QUIT:
			{
				return opresult_fail;
			}
			default:
				break;
			}
		}

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplatform_sdl::main_window_position(unsigned* x, unsigned* y)
	{
		int _x, _y;

		SDL_GetWindowPosition(S_WINDOW, &_x, &_y);

		*x = _x;
		*y = _y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplatform_sdl::main_window_size(unsigned* x, unsigned* y)
	{
		int _x, _y;

		SDL_GetWindowSize(S_WINDOW, &_x, &_y);

		*x = _x;
		*y = _y;
	}

} //- sm