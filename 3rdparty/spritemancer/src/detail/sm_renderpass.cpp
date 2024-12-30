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
			cfg.m_flags = 0;
			cfg.m_rendertarget_ratio = framebuffer_ratio_equal;
			cfg.m_view_mode = view_mode_sequential;
			cfg.m_program = algorithm::hash("smain_renderpass_program");
			cfg.m_rendertarget = algorithm::hash("smain_renderpass_rendertarget");

			return cfg;
		}

	} //- renderpass

} //- sm
