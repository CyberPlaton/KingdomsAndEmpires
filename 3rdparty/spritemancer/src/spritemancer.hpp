#pragma once
#include "detail/sm_config.hpp"
#include "detail/sm_platform_sdl2.hpp"
#if CORE_PLATFORM_WINDOWS
#include "detail/sm_renderer_dx11.hpp"
#else
#include "detail/sm_renderer_opengl33.hpp"
#endif

namespace sm
{
	//- Interface class for application to derive from and implement only three basic functions
	//------------------------------------------------------------------------------------------------------------------------
	class iapp
	{
	protected:
		virtual ~iapp() = default;
		virtual bool on_init() = 0;
		virtual void on_update(float) = 0;
		virtual void on_shutdown() = 0;
	};

} //- sm