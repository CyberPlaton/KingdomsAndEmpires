#include "ecs_world_context_holder.hpp"
#include "ecs_world_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	flecs::world& iworld_context_holder::world()
	{
		return cworld_manager::instance().current();
	}

} //- ecs