#include "ecs_entity.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	centity::centity(flecs::entity& e, flecs::world& w) :
		iworld_context_holder(w), m_self(e)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	centity::centity(flecs::entity& e) :
		iworld_context_holder(e.world()), m_self(e)
	{
	}

} //- ecs