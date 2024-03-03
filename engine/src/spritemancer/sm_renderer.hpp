#pragma once
#include "sm_internal.hpp"

namespace sm
{
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