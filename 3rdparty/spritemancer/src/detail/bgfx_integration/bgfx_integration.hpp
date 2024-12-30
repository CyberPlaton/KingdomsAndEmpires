#pragma once
#include "../sm_config.hpp"

namespace sm
{
	namespace graphics
	{
		namespace detail
		{
			bool check_available_transient_buffers(unsigned vertex_count, const bgfx::VertexLayout& layout, unsigned index_count);

		} //- detail

	} //- graphics

} //- sm
