#pragma once
#include "../sm_config.hpp"
#include "../renderers/sm_renderer_raylib.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cos_raylib final : public ios
	{
	public:
		cos_raylib() = default;
		~cos_raylib() = default;
		opresult init() override final;									//- create and init of client application
		opresult shutdown() override final;								//- destroy and clean of client application
		opresult init_gfx(unsigned w, unsigned h,
			bool fullscreen, bool vsync) override final;				//- create graphical context
		opresult init_mainwindow(stringview_t title,
			unsigned w, unsigned h, bool fullscreen) override final;	//- create application main window
		opresult process_events() override final;						//- process one hardware event
		void main_window_position(unsigned* x, unsigned* y) override final;
		void main_window_size(unsigned* x, unsigned* y) override final;
		float frametime() const override final;

		RTTR_ENABLE(ios);
	};

} //- sm