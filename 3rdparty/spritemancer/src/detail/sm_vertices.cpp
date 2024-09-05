#include "sm_vertices.hpp"

namespace sm
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		void vertices_release_function(void* p, void* /*user_data*/)
		{
		}

	} //- detail

	namespace vertex_layouts
	{
		//------------------------------------------------------------------------------------------------------------------------
		bool spostexcoord::init()
		{
			S_LAYOUT.begin()
					.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
					.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
					.end();

			S_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

			return bgfx::isValid(S_HANDLE);
		}

		//------------------------------------------------------------------------------------------------------------------------
		spostexcoord spostexcoord::make(float x, float y, float u, float v)
		{
			return { vec3_t(x, y, 0.0f), vec2_t(u, v) };
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool spostexcoordcolor::init()
		{
			S_LAYOUT.begin()
					.add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
					.add(bgfx::Attrib::TexCoord0,	2, bgfx::AttribType::Float)
					.add(bgfx::Attrib::Color0,		4, bgfx::AttribType::Uint8, true)
					.end();

			S_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

			return bgfx::isValid(S_HANDLE);
		}

		//------------------------------------------------------------------------------------------------------------------------
		spostexcoordcolor spostexcoordcolor::make(float x, float y, float u, float v, unsigned abgr)
		{
			return { vec3_t(x, y, 0.0f), vec2_t(u, v), abgr};
		}

	} //- vertex_layouts

} //- sm