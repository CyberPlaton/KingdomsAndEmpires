#pragma once
#include "sm_config.hpp"
#if CORE_PLATFORM_WINDOWS
#include "sm_renderer_dx11.hpp"
#else
#include "sm_renderer_opengl33.hpp"
#endif

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cplatform_sdl final : public iplatform
	{
	public:
		cplatform_sdl() = default;
		~cplatform_sdl() = default;
		opresult init() override final;						//- create and init of client application
		opresult shutdown() override final;					//- destroy and clean of client application
		opresult init_on_thread() override final;			//- create and init on internal 'engine' thread, where rendering and main update happens
		opresult shutdown_on_thread() override final;		//- destroy and clean on internal 'engine' thread, where rendering and main update happens
		opresult init_gfx(bool fullscreen, bool vsync) override final;//- create graphical context

		opresult init_mainwindow(stringview_t title,
			unsigned x, unsigned y, unsigned w, unsigned h,
			bool fullscreen) override final;			//- create application main window

		opresult optional_init_event_mainloop() override final;//- process hardware events in a loop; use where required
		opresult process_event() override final;			//- process one hardware event
	};

} //- sm