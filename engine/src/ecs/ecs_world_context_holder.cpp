#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	iworld_context_holder::iworld_context_holder(ref_t<flecs::world> w) :
		m_world(w)
	{
	}

} //- ecs