#pragma once
#include "detail/sm_config.hpp"
#include "detail/sm_platform_sdl2.hpp"

namespace sm
{
	opresult configure(iapp* app, void* config, int argc, char* argv[]);

	opresult init(stringview_t title, unsigned x, unsigned y, unsigned w, unsigned h, bool fullscreen, bool vsync);

	opresult run();

	unsigned create_layer();

	bool bind_layer(unsigned i);

	void sm_logger(core::error_report_function_t callback);

} //- sm