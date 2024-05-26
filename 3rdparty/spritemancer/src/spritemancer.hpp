#pragma once
#include "detail/sm_config.hpp"

namespace sm
{
	opresult configure(iapp* app, void* config, int argc, char* argv[]);

	opresult init(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync);

	opresult run();

#pragma region layer_management
	//- Create a new layer for rendering. Everything rendered on this layer are drawn on a separate render target and
	//- are then drawn as a whole one previously drawn render targets (layers)
	//------------------------------------------------------------------------------------------------------------------------
	unsigned create_layer();

#pragma endregion

#pragma region primitive_rendering
	void draw_line(unsigned layer, const vec2_t& start, const vec2_t& end, float thick, const core::scolor& color);

	void draw_circle(unsigned layer, const vec2_t& center, float radius, const core::scolor& color);

	void draw_rect(unsigned layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color);
#pragma endregion

#pragma region sprite_rendering
	//------------------------------------------------------------------------------------------------------------------------
	void draw_placeholder(unsigned layer, const vec2_t& position, const vec2_t& scale = {1.0f, 1.0f},
		const core::scolor& tint = {255, 255, 255, 255});

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, const srenderable& renderable, const vec2_t& scale,
		const core::scolor& tint);
#pragma endregion

#pragma region utility_and_helpers
	//- Set internal logging callback
	//------------------------------------------------------------------------------------------------------------------------
	void set_logger(core::error_report_function_t callback);
#pragma endregion

} //- sm