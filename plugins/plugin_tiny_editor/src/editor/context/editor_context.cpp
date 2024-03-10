#include "editor_context.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	ccontext::ccontext() :
		m_inspected_entity(core::cuuid::C_INVALID_UUID), m_mainmenu_height(0.0f)
	{
	}

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