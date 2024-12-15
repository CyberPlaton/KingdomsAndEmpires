#pragma once
#include "sm_config.hpp"
#include "sm_vertices.hpp"

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
	class cspritebatch final : public detail::cmesh<sposition_color_texcoord_vertex>
	{
	public:
		cspritebatch();
		~cspritebatch() = default;

		void begin();
		void draw();
		void end();

	private:
		texture_handle_t m_previous_texture = MAX(texture_handle_t);
	};

} //- sm