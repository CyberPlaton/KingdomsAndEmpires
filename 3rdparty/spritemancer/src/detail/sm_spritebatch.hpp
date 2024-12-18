#pragma once
#include "sm_config.hpp"
#include "sm_buffer.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sposition_color_texcoord_vertex final : detail::ivertex_type
	{
		DECLARE_VERTEX_TYPE(sposition_color_texcoord_vertex);

		static void init();
		static void make_vertex(core::cany& buffer, float x, float y, float z, float u, float v, unsigned abgr);

		float m_x = 0.0f;
		float m_y = 0.0f;
		float m_z = 0.0f;
		float m_u = 0.0f;
		float m_v = 0.0f;
		unsigned m_abgr = 0;

		RTTR_ENABLE(detail::ivertex_type);
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
