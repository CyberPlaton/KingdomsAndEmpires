#pragma once
#include "sm_renderer.hpp"

namespace sm
{
	bool init(cwindow::sconfig& cfg);
	void shutdown();

	//- Order of declaration is expected order of calling
	void begin_drawing();
	void end_frame();
	void ui_frame();
	void end_ui_frame();
	void end_drawing();

} //- sm