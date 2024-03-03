#include "sm_renderer.hpp"
#include "sm_embedded_shaders.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	crenderer::~crenderer()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, texture_t texture, material_t material, float rotation,
		const vec2_t& scale, const core::srect& rect, const core::scolor& color, bool flipx, bool flipy)
	{
		internal::irenderer::draw_sprite(layer, position, texture, scale, rotation, material, rect, color, flipx, flipy);
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
		internal::irenderer::draw_spriteatlas_sprite(layer, position, rotation, scale, atlas, subtexture, color, flipx, flipy);
	}

// 	bool sort_draw_command_by_technique(const sdraw_command& a, const sdraw_command& b)
// 	{
// 		return a.m_material.m_technique.idx < b.m_material.m_technique.idx;
// 	}
// 
// 	bool sort_draw_command_by_texture(const sdraw_command& a, const sdraw_command& b)
// 	{
// 		return a.m_material.m_texture.idx < b.m_material.m_texture.idx;
// 	}
// 
// 	void sort_draw_commands(vector_t< sdraw_command >& dc)
// 	{
// 		sort_draw_commands(dc, 0, dc.size(), sorting_criteria_technique);
// 
// 		auto x = 0, y = 0;
// 		for (auto ii = 0; ii < dc.size(); ii++)
// 		{
// 			if (ii + 1 == dc.size())
// 			{
// 				sort_draw_commands(dc, x, dc.size(), sorting_criteria_texture);
// 			}
// 			else if (dc[ii].m_material.m_technique.idx != dc[ii + 1].m_material.m_technique.idx)
// 			{
// 				y = ii + 1;
// 				sort_draw_commands(dc, x, y, sorting_criteria_texture);
// 				x = y;
// 			}
// 		}
// 	}
// 
// 	void sort_draw_commands(vector_t< sdraw_command >& dc, u32 begin, u32 end, sorting_criteria criteria)
// 	{
// 		switch (criteria)
// 		{
// 		case sorting_criteria_technique:
// 		{
// 			std::sort(dc.begin() + begin, dc.begin() + end, sort_draw_command_by_technique);
// 			break;
// 		}
// 		case sorting_criteria_texture:
// 		{
// 			std::sort(dc.begin() + begin, dc.begin() + end, sort_draw_command_by_texture);
// 			break;
// 		}
// 		}
// 	}

} //- sm