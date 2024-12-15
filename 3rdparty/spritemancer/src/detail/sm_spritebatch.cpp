#include "sm_spritebatch.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cspritebatch::cspritebatch()
	{
		set_buffer_type(detail::imesh::buffer_type_transient);
	}

} //- sm