#include "sm_spritebatch.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cspritebatch::cspritebatch()
	{
		m_buffer.set_buffer_type(detail::buffer_type_transient);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspritebatch::begin()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspritebatch::draw(texture_handle_t texture, const vec2_t& position, const core::scolor& color,
		float rotation, const vec2_t& scale, const vec2_t& origin, const core::srect& source)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspritebatch::end()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void sposition_color_texcoord_vertex::init()
	{
		S_LAYOUT.begin()
			.add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0,	2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,		4, bgfx::AttribType::Uint8, true)
			.end();

		S_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

		CORE_ASSERT(bgfx::isValid(S_HANDLE), "Invalid operation. Failed to initialize vertex type 'sposition_color_texcoord_vertex'!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	void sposition_color_texcoord_vertex::make_vertex(core::cany& buffer, float x, float y, float z, float u, float v, unsigned abgr)
	{
		auto& data = buffer.get<vector_t<sposition_color_texcoord_vertex>>();
		data.emplace_back(x, y, z, u, v, abgr);
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	REGISTER_VERTEX_TYPE(sposition_color_texcoord_vertex);
}
