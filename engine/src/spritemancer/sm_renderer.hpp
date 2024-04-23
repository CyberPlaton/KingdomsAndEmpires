#pragma once
#include "sm_internal.hpp"

namespace sm
{
	//- Deferred renderer class. Can be created to start rendering commands
	//- that are submitted when renderer goes out of scope. The actual rendering is done on main thread when the frame ends.
	//- Depth fighting is avoided through layers, however rendering multiple sprites on same layer
	//- and on same position is still an issue (would be an issue on single threaded renderers too).
	//------------------------------------------------------------------------------------------------------------------------
	class crenderer : public detail::irenderer
	{
	public:
		crenderer() = default;
		~crenderer() = default;

		void use_camera(renderlayer_t layer, const vec2_t& position, const vec2_t& offset, float rotation, float zoom, const core::scolor& clearcolor);

		void draw_line(renderlayer_t layer, const vec2_t& start, const vec2_t& end, float thick, const core::scolor& color);

		void draw_circle(renderlayer_t layer, const vec2_t& center, float radius, const core::scolor& color);

		void draw_rect(renderlayer_t layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color);

		void draw_text();

		void draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
			const vec2_t& scale, const core::srect& rect, const core::scolor& color, bool flipx, bool flipy);

		void draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
			const vec2_t& scale, const core::srect& rect, const core::scolor& color);

		void draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
			const vec2_t& scale, const core::srect& rect);

		void draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
			const vec2_t& scale);

		void draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation);
		void draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material);
		void draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture);

		void draw_spriteatlas_sprite(renderlayer_t layer, const vec2_t& position, float rotation, const vec2_t& scale,
			spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy);
	};

} //- sm