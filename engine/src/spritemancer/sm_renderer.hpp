#pragma once
#include "sm_internal.hpp"

namespace sm
{
	//- Renderpath drawing to a separate rendertarget. The layer represents the order of final combining of rendertargets,
	//- where 0 is bottom layer and n is top layer.
	//- On creating of the renderpath the rendertarget is cleared by default to a blank color.
	//------------------------------------------------------------------------------------------------------------------------
	class crenderpath final
	{
	public:
		crenderpath(renderlayer_t layer, unsigned w = 0, unsigned h = 0);
		~crenderpath();

		void clear(const core::scolor& color);

		void viewrect(unsigned x, unsigned y, unsigned w, unsigned h);

		void begin_camera(const vec2_t& position, const vec2_t& offset, float zoom, float rotation);
		void end_camera();

		void begin_blend_mode(blending_mode mode);
		void end_blend_mode();

		void begin_material(material_t material);
		void end_material();

		void draw_line(const vec2_t& start, const vec2_t& end, float thick, const core::scolor& color);

		void draw_circle(const vec2_t& center, float radius, const core::scolor& color);

		void draw_rect(const vec2_t& position, const vec2_t& dimension, const core::scolor& color);

		void draw_text();

		void draw_sprite(const vec2_t& position, texture_t texture, float rotation,
			const vec2_t& scale, const core::srect& rect, const vec2_t& origin, const core::scolor& color, bool flipx, bool flipy);

		void draw_sprite(const vec2_t& position, texture_t texture, float rotation,
			const vec2_t& scale, const core::srect& rect, const core::scolor& color, bool flipx, bool flipy);

		void draw_sprite(const vec2_t& position, texture_t texture, float rotation,
			const vec2_t& scale, const core::srect& rect, const core::scolor& color);

		void draw_sprite(const vec2_t& position, texture_t texture, float rotation,
			const vec2_t& scale, const core::srect& rect);

		void draw_sprite(const vec2_t& position, texture_t texture, float rotation,
			const vec2_t& scale);

		void draw_sprite(const vec2_t& position, texture_t texture, float rotation);
		void draw_sprite(const vec2_t& position, texture_t texture);
		
		void draw_spriteatlas_sprite(const vec2_t& position, float rotation, const vec2_t& scale,
			spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy);

	private:
		core::srect m_viewrect;
		rendertarget_t m_target;
		renderlayer_t m_layer;
		material_t m_current;
	};

} //- sm