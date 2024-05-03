#pragma once
#include "sm_renderer.hpp"

namespace sm
{
	bool init(cwindow::sconfig& cfg);
	void shutdown();

	//- Order of declaration is expected order of calling
	void frame();
	void end();

} //- sm
