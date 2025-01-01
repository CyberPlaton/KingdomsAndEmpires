#include "sm_vertex_layouts.hpp"

namespace sm::vertexlayout
{
	//------------------------------------------------------------------------------------------------------------------------
	bool sposcolortexcoord::init()
	{
		S_LAYOUT.begin(bgfx::getRendererType())
			.add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,		4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::TexCoord0,	2, bgfx::AttribType::Float)
			.end();

		S_LAYOUT_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

		return bgfx::isValid(S_LAYOUT_HANDLE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bgfx::VertexLayout sposcolortexcoord::vertex_layout()
	{
		return S_LAYOUT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::vertex_layout_handle_t sposcolortexcoord::vertex_layout_handle()
	{
		return S_LAYOUT_HANDLE.idx;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sposcolortexcoord sposcolortexcoord::make(float x, float y, float z, unsigned abgr, float u, float v)
	{
		return sposcolortexcoord{x, y, z, abgr, u, v};
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool spostexcoord::init()
	{
		S_LAYOUT.begin(bgfx::getRendererType())
			.add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0,	2, bgfx::AttribType::Float)
			.end();

		S_LAYOUT_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

		return bgfx::isValid(S_LAYOUT_HANDLE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bgfx::VertexLayout spostexcoord::vertex_layout()
	{
		return S_LAYOUT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::vertex_layout_handle_t spostexcoord::vertex_layout_handle()
	{
		return S_LAYOUT_HANDLE.idx;
	}

	//------------------------------------------------------------------------------------------------------------------------
	spostexcoord spostexcoord::make(float x, float y, float z, float u, float v)
	{
		return spostexcoord{ x, y, z, u, v };
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sposcolor::init()
	{
		S_LAYOUT.begin(bgfx::getRendererType())
			.add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,		4, bgfx::AttribType::Uint8, true)
			.end();

		S_LAYOUT_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

		return bgfx::isValid(S_LAYOUT_HANDLE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bgfx::VertexLayout sposcolor::vertex_layout()
	{
		return S_LAYOUT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::vertex_layout_handle_t sposcolor::vertex_layout_handle()
	{
		return S_LAYOUT_HANDLE.idx;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sposcolor sposcolor::make(float x, float y, float z, unsigned abgr)
	{
		return sposcolor{ x, y, z, abgr };
	}

} //- sm::vertexlayout