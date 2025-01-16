#include "sm_renderpass.hpp"
#include "sm_rendertarget.hpp"
#include "sm_program.hpp"

namespace sm
{
	namespace renderpass
	{
		//------------------------------------------------------------------------------------------------------------------------
		sconfig smain_renderpass::config()
		{
			sconfig cfg;

			cfg.m_id = 0;
			cfg.m_flags = renderpass_flag_no_framebuffer | renderpass_flag_no_program;
			cfg.m_rendertarget_ratio = framebuffer_ratio_equal;
			cfg.m_view_mode = view_mode_sequential;
			cfg.m_program = C_INVALID_HANDLE;
			cfg.m_rendertarget = C_INVALID_HANDLE;

			return cfg;
		}

	} //- renderpass

} //- sm
