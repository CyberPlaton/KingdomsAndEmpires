#pragma once
#include "../sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cos_headless final : public ios
	{
	public:
		cos_headless() = default;
		~cos_headless() = default;
		opresult init() override final { return opresult_ok; }
		opresult shutdown() override final { return opresult_ok; }
		opresult init_gfx(unsigned w, unsigned h,
			bool fullscreen, bool vsync) override final { return opresult_ok; }

		opresult init_mainwindow(stringview_t title,
			unsigned w, unsigned h, bool fullscreen) override final { return opresult_ok; }

		opresult optional_init_event_mainloop() override final { return opresult_ok; }
		opresult optional_process_event() override final { return opresult_ok; }

		void main_window_position(unsigned* x, unsigned* y) override final {}
		void main_window_size(unsigned* x, unsigned* y) override final {}

		RTTR_ENABLE(ios);
	};

} //- sm