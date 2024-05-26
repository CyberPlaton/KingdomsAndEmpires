#pragma once
#include "../sm_config.hpp"
#include "../renderers/sm_renderer_raylib.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cplatform_raylib final : public iplatform
	{
	public:
		cplatform_raylib() = default;
		~cplatform_raylib() = default;
		opresult init() override final;						//- create and init of client application
		opresult shutdown() override final;					//- destroy and clean of client application
		opresult init_gfx(unsigned w, unsigned h,
			bool fullscreen, bool vsync) override final;	//- create graphical context

		opresult init_mainwindow(stringview_t title,
			unsigned w, unsigned h, bool fullscreen) override final; //- create application main window

		opresult optional_init_event_mainloop() override final;	//- process hardware events in a loop; use where required
		opresult optional_process_event() override final;		//- process one hardware event

		void main_window_position(unsigned* x, unsigned* y) override final;
		void main_window_size(unsigned* x, unsigned* y) override final;

		RTTR_ENABLE(iplatform);
	};

} //- sm

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cplatform_raylib)
	{
		rttr::cregistrator<cplatform_raylib>("cplatform_raylib")
			;
	};

} //- sm