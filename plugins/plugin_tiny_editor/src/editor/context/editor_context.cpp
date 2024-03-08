#include "editor_context.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	float ccontext::window_width()
	{
		return SCAST(float, raylib::GetScreenWidth());
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ccontext::window_height()
	{
		return SCAST(float, raylib::GetScreenHeight());
	}

} //- editor