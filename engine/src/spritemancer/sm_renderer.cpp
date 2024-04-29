#include "sm_renderer.hpp"
#include "sm_embedded_shaders.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::begin_camera(const vec2_t& position, const vec2_t& offset, float zoom, float rotation)
	{
		raylib::Camera2D frame_camera{ 0 };
		frame_camera.target.x = position.x;
		frame_camera.target.y = position.y;
		frame_camera.offset.x = offset.x;
		frame_camera.offset.y = offset.y;
		frame_camera.rotation = rotation;
		frame_camera.zoom = zoom;

		raylib::BeginMode2D(frame_camera);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::end_camera()
	{
		raylib::EndMode2D();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::begin_material(material_t material)
	{
		const auto& mat = ctx().sm().get(material);

		mat.bind();

		m_current = material;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::end_material()
	{
		if (algorithm::is_valid_handle(m_current))
		{
			const auto& mat = ctx().sm().get(m_current);

			mat.unbind();

			m_current = invalid_handle_t;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_line(const vec2_t& start, const vec2_t& end,
		float thick, const core::scolor& color)
	{
		raylib::DrawLineEx({ start.x, start.y }, { end.x, end.y }, thick, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_circle(const vec2_t& center, float radius, const core::scolor& color)
	{
		raylib::DrawCircle(center.x, center.y, radius, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_rect(const vec2_t& position, const vec2_t& dimension, const core::scolor& color)
	{
		raylib::DrawRectangleV({ position.x, position.y }, { dimension.x, dimension.y }, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(const vec2_t& position, texture_t texture, float rotation,
		const vec2_t& scale, const core::srect& rect, const core::scolor& color, bool flipx, bool flipy)
	{
		auto __rect = rect;
		if (flipx) __rect.m_w = (-__rect.m_w);
		if (flipy) __rect.m_h = (-__rect.m_h);

		const auto& tex = ctx().tm().get(texture);

		//- TODO: we do not use width and height from command transform, why
		raylib::Rectangle src = { __rect.x(), __rect.y() , __rect.w() , __rect.h() };
		raylib::Rectangle dst = { position.x, position.y, src.width, src.height };

		//- TODO: origin should be variable, probably a component thing that should be redirected to here
		raylib::DrawTexturePro(tex.texture(), src, dst, { 0.0f, 0.0f }, rotation, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(const vec2_t& position, texture_t texture, float rotation,
		const vec2_t& scale, const core::srect& rect, const core::scolor& color)
	{
		draw_sprite(position, texture, rotation, scale, rect, color, false, false);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(const vec2_t& position, texture_t texture, float rotation,
		const vec2_t& scale, const core::srect& rect)
	{
		draw_sprite(position, texture, rotation, scale, rect, C_COLOR_WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(const vec2_t& position, texture_t texture, float rotation,
		const vec2_t& scale)
	{
		draw_sprite(position, texture, rotation, scale, C_DEFAULT_RECT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(const vec2_t& position, texture_t texture, float rotation)
	{
		draw_sprite(position, texture, rotation, C_DEFAULT_SCALE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(const vec2_t& position, texture_t texture)
	{
		draw_sprite(position, texture, 0.0f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_spriteatlas_sprite(const vec2_t& position, float rotation, const vec2_t& scale,
		spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy)
	{
		//internal::irenderer::draw_spriteatlas_sprite(layer, position, rotation, scale, atlas, subtexture, color, flipx, flipy);
	}

} //- sm