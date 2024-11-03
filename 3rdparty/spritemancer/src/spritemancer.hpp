#pragma once
#include "detail/sm_config.hpp"
#include "detail/sm_resource_manager.hpp"
#include "detail/sm_layer.hpp"
#include <argparse.h>

namespace sm
{
	opresult	prepare(iapp* app, void* config);
	opresult	run(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync);
	scontext&	ctx();
	void		set_logger(core::error_report_function_t callback);
	void		configure_args(argparse::ArgumentParser& args);

	//-- FIXME: in raylib we have only one window, so this function makes sense. When transitioning to bgfx,
	//-- we should have a window service that allows for querying of the main window size and any window size.
	vec2_t		window_size();

	//- Create a new layer for rendering. Everything rendered on this layer are drawn on a separate render target and
	//- are then drawn as a whole one previously drawn render targets (layers)
	unsigned	create_layer();
	slayer&		get_layer(unsigned layer);

	void draw_line(unsigned layer, const vec2_t& start, const vec2_t& end, float thick, const core::scolor& color);
	void draw_circle(unsigned layer, const vec2_t& center, float radius, const core::scolor& color);
	void draw_rect(unsigned layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color);
	void draw_placeholder(unsigned layer, const vec2_t& position, const vec2_t& scale = {1.0f, 1.0f}, const core::scolor& tint = {255, 255, 255, 255});
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture);
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint);
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation);
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation, const vec2_t& scale);
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation, const vec2_t& scale, shader_handle_t shader);
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation, const vec2_t& scale, shader_handle_t shader, const srenderstate& state);
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation, const vec2_t& scale, shader_handle_t shader, const srenderstate& state, const vec2_t& origin);
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation, const vec2_t& scale, shader_handle_t shader, const srenderstate& state, const vec2_t& origin, const core::srect& source);

} //- sm