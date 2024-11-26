#pragma once
#include "sm_config.hpp"
#include "../sm_rendertarget.hpp"
#include "../sm_shader.hpp"

namespace sm
{
	//- Description of a rendering layer. Some of the data becomes only relevant when appropriate flags are set
	//------------------------------------------------------------------------------------------------------------------------
	struct srendering_layer
	{
		vector_t<ccommand> m_commands;
		ccamera m_camera;					//- optional: camera to be used when rendering
		unsigned m_flags = 0;				//- bitwise concated layer_flags
		bool m_show = false;
		unsigned m_id = 0;
	};

} //- sm