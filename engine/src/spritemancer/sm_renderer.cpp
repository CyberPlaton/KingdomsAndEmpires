#include "sm_renderer.hpp"
#include "sm_embedded_shaders.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::clear_view(renderlayer_t layer, const core::scolor& color)
	{
		buffer().push(std::move(cdrawcommand::opcode(drawcommand_opcode_clear, core::cany(color), layer)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::use_camera(renderlayer_t layer, const vec2_t& position, const vec2_t& offset,
		float rotation, float zoom)
	{
		buffer().push(std::move(cdrawcommand::camera(position, offset, zoom, rotation, layer)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_line(renderlayer_t layer, const vec2_t& start, const vec2_t& end,
		float thick, const core::scolor& color)
	{
		buffer().push(std::move(cdrawcommand::line(start, end, thick, color, layer)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_circle(renderlayer_t layer, const vec2_t& center, float radius, const core::scolor& color)
	{
		buffer().push(std::move(cdrawcommand::circle(center, radius, color, layer)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_rect(renderlayer_t layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color)
	{
		buffer().push(std::move(cdrawcommand::rectangle(position, dimension, color, layer)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
		const vec2_t& scale, const core::srect& rect, const core::scolor& color, bool flipx, bool flipy)
	{
		auto __rect = rect;
		if (flipx) __rect.m_w = (-__rect.m_w);
		if (flipy) __rect.m_h = (-__rect.m_h);

		buffer().push(std::move(cdrawcommand::sprite(__rect, color, position, scale, rotation, material, texture, layer)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
		const vec2_t& scale, const core::srect& rect, const core::scolor& color)
	{
		draw_sprite(layer, position, texture, material, rotation, scale, rect, color, false, false);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
		const vec2_t& scale, const core::srect& rect)
	{
		draw_sprite(layer, position, texture, material, rotation, scale, rect, C_COLOR_WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
		const vec2_t& scale)
	{
		draw_sprite(layer, position, texture, material, rotation, scale, C_DEFAULT_RECT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation)
	{
		draw_sprite(layer, position, texture, material, rotation, C_DEFAULT_SCALE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material)
	{
		draw_sprite(layer, position, texture, material, 0.0f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture)
	{
		draw_sprite(layer, position, texture, C_DEFAULT_MATERIAL);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_spriteatlas_sprite(renderlayer_t layer, const vec2_t& position, float rotation, const vec2_t& scale,
		spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy)
	{
		//internal::irenderer::draw_spriteatlas_sprite(layer, position, rotation, scale, atlas, subtexture, color, flipx, flipy);
	}

} //- sm