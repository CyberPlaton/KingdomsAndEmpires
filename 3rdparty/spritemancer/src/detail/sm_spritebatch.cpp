#include "sm_spritebatch.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cspritebatch::cspritebatch()
	{
		m_buffer = std::make_shared<detail::cbuffer<sposition_color_texcoord_vertex>>();
		m_buffer->set_buffer_type(detail::ibuffer::buffer_type_transient);
	}

} //- sm