#pragma once
#include <core.h>
#include "bgfx_integration/bgfx.hpp"

namespace sm
{
	namespace vertex_layouts
	{
		//- Vertex with positions and texture coordinates
		//------------------------------------------------------------------------------------------------------------------------
		struct spostexcoord final
		{
			static bool init();
			static spostexcoord make(float x, float y, float u, float v);

			inline static bgfx::VertexLayout S_LAYOUT;
			inline static bgfx::VertexLayoutHandle S_HANDLE;

			vec3_t m_position;
			vec2_t m_uv;
		};

		//- Vertex with positions, texture coordinates and colors
		//------------------------------------------------------------------------------------------------------------------------
		struct spostexcoordcolor final
		{
			static bool init();
			static spostexcoordcolor make(float x, float y, float u, float v, unsigned abgr);

			inline static bgfx::VertexLayout S_LAYOUT;
			inline static bgfx::VertexLayoutHandle S_HANDLE;

			vec3_t m_position;
			vec2_t m_uv;
			unsigned m_abgr;
		};

	} //- vertex_layouts

	//- Responsible for storing vertices and operating on them.
	//------------------------------------------------------------------------------------------------------------------------
	class cvertices final
	{
	public:
		cvertices();
		explicit cvertices(const bgfx::VertexLayout& layout);
		~cvertices();

	private:
		vertex_type m_type;
	};

} //- sm