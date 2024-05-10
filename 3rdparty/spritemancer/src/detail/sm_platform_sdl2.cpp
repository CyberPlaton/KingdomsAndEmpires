#include "sm_platform_sdl2.hpp"

namespace sm
{
	namespace
	{
		SDL_Window* window = nullptr;

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
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::shutdown()
	{
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
	sm::opresult cplatform_sdl::init_gfx(bool fullscreen, bool vsync)
	{
		if (entry::renderer()->init_device(native_window_handle(window), fullscreen, vsync) != opresult_ok)
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cplatform_sdl::init_mainwindow(stringview_t title,
		unsigned x, unsigned y, unsigned w, unsigned h,
		bool fullscreen)
	{
		unsigned flags = SDL_WINDOW_BORDERLESS;

		if (fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		window = SDL_CreateWindow(title.data(), x, y, w, h, flags);

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

		SDL_PollEvent(&e);

		switch (e.type)
		{
		case SDL_QUIT:
		{
			break;
		}
		default:
			break;
		}

		return opresult_ok;
	}

} //- sm