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
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture);

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint);

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation);

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale);

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, program_handle_t program);

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, program_handle_t program, const srenderstate& state);

	//- Note: origin is {0.0f, 0.0f} for top-left and {1.0f, 1.0f} for bottom-right
	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, program_handle_t program, const srenderstate& state, const vec2_t& origin);

	//- Note: source is ranging from 0.0f to 1.0f. To sample from complete texture define rectangle as {0.0f, 0.0f, 1.0f, 1.0f}
	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, program_handle_t program, const srenderstate& state, const vec2_t& origin, const core::srect& source);

#pragma endregion

#pragma region utility_and_helpers
	//- Set internal logging callback
	//------------------------------------------------------------------------------------------------------------------------
	void set_logger(core::error_report_function_t callback);
#pragma endregion

} //- sm