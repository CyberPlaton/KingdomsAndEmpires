#pragma once
#include "sm_config.hpp"
#include "sm_buffer.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sposition_color_texcoord_vertex final
	{
		static bool init();
		static bgfx::VertexLayout vertex_layout();
		static vertex_layout_handle_t vertex_layout_handle();
		static sposition_color_texcoord_vertex make(float x, float y, float z);

		float m_x = 0.0f;
		float m_y = 0.0f;
		float m_z = 0.0f;
		float m_u = 0.0f;
		float m_v = 0.0f;
		unsigned m_abgr = 0;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspritebatch final
	{
	public:
		cspritebatch();
		~cspritebatch() = default;

		void begin();
		void draw(texture_handle_t texture, const vec2_t& position, const core::scolor& color,
			float rotation, const vec2_t& scale, const vec2_t& origin, const core::srect& source);
		void end();

	private:
		detail::cbuffer<sposition_color_texcoord_vertex> m_buffer;
		texture_handle_t m_previous_texture = MAX(texture_handle_t);
	};

} //- sm