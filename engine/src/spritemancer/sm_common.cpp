#include "sm_common.hpp"
#include <plugin_logging.h>

namespace sm
{
	namespace
	{


	} //- unnamed


	//------------------------------------------------------------------------------------------------------------------------
	ctimestep::ctimestep(unsigned target_fps /*= 0*/) :
		m_fps(target_fps == 0 ? 60 : target_fps)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimestep::real_frametime()
	{
		return SCAST(float, sapp_frame_duration());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned ctimestep::real_fps()
	{
		return 1000 * SCAST(unsigned, sapp_frame_duration());
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimestep::custom_frametime() const
	{
		return 1000.0f / SCAST(float, m_fps);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned ctimestep::custom_fps() const
	{
		return m_fps;
	}

} //- sm