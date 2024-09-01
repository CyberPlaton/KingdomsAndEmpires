#pragma once
#include <core.h>
#include "bgfx_integration/bgfx.hpp"
#include "sm_config.hpp"

namespace sm
{
	//- Responsible for defining a rendering layer along with the view id and framebuffer and additional required data.
	//------------------------------------------------------------------------------------------------------------------------
	class cview final
	{
	public:

	private:
		ccamera m_camera;
		crendertarget m_target;
		cprogram m_program;
		bgfx::ViewId m_id;
	};

} //- sm