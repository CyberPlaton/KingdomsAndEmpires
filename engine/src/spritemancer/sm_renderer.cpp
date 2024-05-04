#include "sm_renderer.hpp"
#include "sm_embedded_shaders.hpp"
#include "../ecs/ecs_component.hpp"

namespace sm
{
	namespace
	{
		constexpr stringview_t C_RENDERTARGET_NAME = "RenderTarget";
		static const core::scolor C_COLOR_BLANK = core::scolor(0, 0, 0, 0);

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	crenderpath::crenderpath(renderlayer_t layer, unsigned w /*= 0*/, unsigned h /*= 0*/) :
		m_layer(layer), m_current(invalid_handle_t)
	{
		if (w == 0)
		{
			w = sm::ctx().window().w();
		}
		if (h == 0)
		{
			h = sm::ctx().window().h();
		}

		m_target = sm::ctx().rm().load(fmt::format("{}_{}", C_RENDERTARGET_NAME.data(), layer), w, h);

		const auto& target = sm::ctx().rm().get(m_target);

		target.bind();

		viewrect(0, 0, w, h);
		clear(C_COLOR_BLANK);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crenderpath::~crenderpath()
	{
		end_camera();
		raylib::EndTextureMode();

		sm::ctx().submit(m_layer, m_target, m_viewrect);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::clear(const core::scolor& color)
	{
		raylib::ClearBackground(to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::viewrect(unsigned x, unsigned y, unsigned w, unsigned h)
	{
		m_viewrect = { SCAST(float, x), SCAST(float, y) , SCAST(float, w) , SCAST(float, h) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::begin_blend_mode(blending_mode mode)
	{
		raylib::BeginBlendMode(mode);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::end_blend_mode()
	{
		raylib::EndBlendMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::begin_camera(const vec2_t& position, const vec2_t& offset, float zoom, float rotation)
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
	void crenderpath::end_camera()
	{
		raylib::EndMode2D();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::begin_material(material_t material)
	{
		const auto& mat = ctx().sm().get(material);

		mat.bind();

		m_current = material;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::end_material()
	{
		if (algorithm::is_valid_handle(m_current))
		{
			const auto& mat = ctx().sm().get(m_current);

			mat.unbind();

			m_current = invalid_handle_t;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_line(const vec2_t& start, const vec2_t& end,
		float thick, const core::scolor& color)
	{
		raylib::DrawLineEx({ start.x, start.y }, { end.x, end.y }, thick, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_circle(const vec2_t& center, float radius, const core::scolor& color)
	{
		raylib::DrawCircle(center.x, center.y, radius, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_rect(const vec2_t& position, const vec2_t& dimension, const core::scolor& color)
	{
		raylib::DrawRectangleV({ position.x, position.y }, { dimension.x, dimension.y }, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_sprite(const vec2_t& position, texture_t texture, float rotation, const vec2_t& scale,
		const core::srect& rect, const vec2_t& origin, const core::scolor& color, int flags)
	{
		if (!algorithm::bit_on(flags, ecs::ssprite::flags_visible))
		{
			return;
		}
		auto __rect = rect;
		if (algorithm::bit_on(flags, ecs::ssprite::flags_flipx))
		{
			__rect.m_w = (-__rect.m_w);
		}
		if (algorithm::bit_on(flags, ecs::ssprite::flags_flipy))
		{
			__rect.m_h = (-__rect.m_h);
		}

		const auto& tex = ctx().tm().get(texture);

		//- TODO: as of now, scale is the size of the resulting sprite,
		//- consider making it a scalar value that scales rect.w() and .h()
		//- by its amount
		raylib::Rectangle src = { __rect.x(), __rect.y() , __rect.w() , __rect.h() };
		raylib::Rectangle dst = { position.x, position.y, scale.x, scale.y };

		raylib::DrawTexturePro(tex.texture(), src, dst, { origin.x, origin.y }, rotation, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_sprite(const vec2_t& position, texture_t texture, float rotation,
		const vec2_t& scale, const core::srect& rect, const core::scolor& color, int flags)
	{
		draw_sprite(position, texture, rotation, scale, rect, { scale.x / 2.0f, scale.y / 2.0f }, color, flags);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_sprite(const vec2_t& position, texture_t texture, float rotation,
		const vec2_t& scale, const core::srect& rect, const core::scolor& color)
	{
		draw_sprite(position, texture, rotation, scale, rect, color, ecs::ssprite::flags_visible);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_sprite(const vec2_t& position, texture_t texture, float rotation,
		const vec2_t& scale, const core::srect& rect)
	{
		draw_sprite(position, texture, rotation, scale, rect, C_COLOR_WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_sprite(const vec2_t& position, texture_t texture, float rotation,
		const vec2_t& scale)
	{
		draw_sprite(position, texture, rotation, scale, C_DEFAULT_RECT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_sprite(const vec2_t& position, texture_t texture, float rotation)
	{
		draw_sprite(position, texture, rotation, C_DEFAULT_SCALE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_sprite(const vec2_t& position, texture_t texture)
	{
		draw_sprite(position, texture, 0.0f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderpath::draw_spriteatlas_sprite(const vec2_t& position, float rotation, const vec2_t& scale,
		spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy)
	{
		//internal::irenderer::draw_spriteatlas_sprite(layer, position, rotation, scale, atlas, subtexture, color, flipx, flipy);
	}

} //- sm