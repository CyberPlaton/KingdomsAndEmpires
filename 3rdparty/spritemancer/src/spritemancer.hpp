#pragma once
#include "detail/sm_config.hpp"

namespace sm
{
	opresult configure(iapp* app, void* config, int argc, char* argv[]);

	opresult init(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync);

	opresult run();

	unsigned create_layer();

	bool bind_layer(unsigned i);

	void sm_logger(core::error_report_function_t callback);

	void draw_placeholder(const vec2_t& position, const vec2_t& scale = {1.0f, 1.0f}, const core::scolor& tint = {255, 255, 255, 255});

	void draw_texture(const vec2_t& position, const srenderable& renderable, const vec2_t& scale, const core::scolor& tint);

} //- sm