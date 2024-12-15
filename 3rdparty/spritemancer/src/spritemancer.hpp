#pragma once
#include "detail/sm_config.hpp"
#include "detail/sm_resource_manager.hpp"

namespace sm
{
	namespace renderpass
	{
		using renderpass_flags_t = int;

		//------------------------------------------------------------------------------------------------------------------------
		enum renderpass_flag : uint8_t
		{

		};

		//------------------------------------------------------------------------------------------------------------------------
		struct sconfig final
		{
			string_t m_name;
			renderpass_flags_t m_flags = 0;
		};

		//- This is a dummy rendering pass to show how one should be defined. While creating you must not inherit from it.
		//- Define all required functions and reflect them to RTTR using the macro REGISTER_RENDERPASS().
		//------------------------------------------------------------------------------------------------------------------------
		struct srenderpass final
		{
			static constexpr rttr::string_view C_RENDERPASS_CONFIG_FUNC_NAME = "config";
			static constexpr array_t<rttr::string_view, 1> C_RENDERPASS_FUNC_NAMES = { C_RENDERPASS_CONFIG_FUNC_NAME };

			static sconfig config() { return {}; }

			RTTR_ENABLE();
		};

	} //- renderpass


	opresult	prepare(iapp* app, void* config);
	opresult	run(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync);
	void		set_logger(core::error_report_function_t callback);
	void		configure_args(const args_ref_t& args);

	//-- FIXME: in raylib we have only one window, so this function makes sense. When transitioning to bgfx,
	//-- we should have a window service that allows for querying of the main window size and any window size.
	vec2_t		window_size();
	vec2_t		window_position();

	//- Create a new layer for rendering. Everything rendered on this layer are drawn on a separate render target and
	//- are then drawn as a whole one previously drawn render targets (layers)
	unsigned			create_layer();

	//- Sync point of application camera data and spritemancer internal camera data used per frame drawing
	void update_frame_camera(const vec2_t& position, const vec2_t& offset, float zoom, float rotation);

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