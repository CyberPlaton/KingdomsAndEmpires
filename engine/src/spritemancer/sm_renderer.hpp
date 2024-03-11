#pragma once
#include "sm_internal.hpp"

namespace sm
{
	//- Deferred renderer class. Can be created to start rendering commands
	//- that are submitted when renderer goes out of scope. The actual rendering is done on main thread
	//- when the frame ends.
	//- Depth fighting is avoided through layers, however rendering multiple sprites on same layer
	//- and on same position could be an issue (would be an issue on single threaded renderers too). 
	//------------------------------------------------------------------------------------------------------------------------
	class crenderer : public internal::irenderer
	{
	public:
		crenderer() = default;
		~crenderer();

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

	private:
		internal::ccommand_buffer m_commands;
	};

} //- sm