#pragma once
#include "detail/sm_config.hpp"
#include "detail/sm_resource_manager.hpp"

namespace sm
{
	opresult configure(iapp* app, void* config, int argc, char* argv[]);

	opresult init(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync);

	opresult run();

	//-- FIXME: in raylib we have only one window, so this function makes sense. When transitioning to bgfx,
	//-- we should have a window service that allows for querying of the main window size and any window size.
	vec2_t window_size();

	void draw_primitives(unsigned layer, const cvertices& vertices, const indices_t& indices, program_handle_t program,
		const core::scolor& color, srenderstate state);

	void draw_triangles(unsigned layer, const cvertices& vertices, const indices_t& indices, program_handle_t program,
		const core::scolor& color, srenderstate state);

	//- Set internal logging callback
	//------------------------------------------------------------------------------------------------------------------------
	void set_logger(core::error_report_function_t callback);

} //- sm