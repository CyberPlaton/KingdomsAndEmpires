#include "bgfx_integration.hpp"
#include "../sm_rendertarget.hpp"
#include "../sm_program.hpp"

namespace sm
{
	namespace graphics
	{
		namespace detail
		{
			//------------------------------------------------------------------------------------------------------------------------
			bool check_available_transient_buffers(unsigned vertex_count, const bgfx::VertexLayout& layout, unsigned index_count)
			{
				return vertex_count == bgfx::getAvailTransientVertexBuffer(vertex_count, layout) &&
					(0 == index_count || index_count == bgfx::getAvailTransientIndexBuffer(index_count));
			}

		} //- detail

	} //- graphics

} //- sm
