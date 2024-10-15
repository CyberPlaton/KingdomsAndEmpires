#pragma once
#include "sm_config.hpp"
#include "../sm_rendertarget.hpp"
#include "../sm_shader.hpp"

namespace sm
{
	//- Description of a rendering layer. Some of the data becomes only relevant when appropriate flags are set
	//------------------------------------------------------------------------------------------------------------------------
	struct slayer
	{
		crendertarget m_target;
		vector_t<ccommand> m_commands;
		ccamera m_camera;					//- optional: camera to be used when rendering
		cshader m_shader;					//- optional: shader used to render the layer render texture on previous layers
		core::scolor m_combine_tint;		//- color put over layer render texture when drawing on previous layers
		core::scolor m_clear_tint;			//- color used to clear the layer render texture before drawing
		vec2_t m_position = { 0.0f, 0.0f }; //- optional: where to draw the render target when combining; by default we cover whole screen
		vec2_t m_scale = { 1.0f, 1.0f };	//- optional: scaling of the render target; by default normal scale
		vec2_t m_origin = { 0.0f, 0.0f };	//- optional: origin of the render target; by default top-left
		unsigned m_flags = 0;				//- bitwise concated layer_flags
		bool m_show = false;
		unsigned m_id = 0;
	};

} //- sm