#include "ecs_component.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	void sidentifier::show_ui(flecs::entity e)
	{
		auto* identifier = e.get_mut<sidentifier>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void stransform::show_ui(flecs::entity e)
	{
		auto* transform = e.get_mut<stransform>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void shierarchy::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void sanimation::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ssprite::show_ui(flecs::entity e)
	{

	}

} //- ecs