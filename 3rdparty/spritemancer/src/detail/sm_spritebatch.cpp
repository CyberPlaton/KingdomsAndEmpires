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

} //- sm