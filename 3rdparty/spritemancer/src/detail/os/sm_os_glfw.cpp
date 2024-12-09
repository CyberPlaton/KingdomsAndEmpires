#include "sm_os_glfw.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		void* native_window_handle(GLFWwindow* window)
		{
#if CORE_PLATFORM_WINDOWS
			return glfwGetWin32Window(window);
#elif CORE_PLATFORM_LINUX
			return (void*)(uintptr_t)glfwGetX11Window(window);
#elif CORE_PLATFORM_OSX
			return glfwGetCocoaWindow(window);
#else
			return nullptr;
#endif
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::init()
	{
		//- TODO: set error callback and key callback and other callbacks
		if (!glfwInit())
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::shutdown()
	{
		glfwDestroyWindow(S_WINDOW);
		glfwTerminate();
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::init_gfx(unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		if (entry::renderer()->init_device(native_window_handle(S_WINDOW), w, h, fullscreen, vsync) != opresult_ok)
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::init_mainwindow(stringview_t title, unsigned w, unsigned h,
		bool fullscreen)
	{
		unsigned flags = 0;

		if (fullscreen)
		{
		}

		//- set window hints that are required before creation
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		S_WINDOW = glfwCreateWindow(w, h, title.data(), nullptr, nullptr);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::optional_init_event_mainloop()
	{
		//- empty
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cos_glfw::optional_process_event()
	{
		//- process queued events and call associated callbacks that we can handle
		glfwPollEvents();

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::main_window_position(unsigned* x, unsigned* y)
	{
		int _x, _y;

		glfwGetWindowPos(S_WINDOW, &_x, &_y);

		*x = _x;
		*y = _y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cos_glfw::main_window_size(unsigned* x, unsigned* y)
	{
		int _x, _y;

		glfwGetWindowSize(S_WINDOW, &_x, &_y);

		*x = _x;
		*y = _y;
	}

} //- sm