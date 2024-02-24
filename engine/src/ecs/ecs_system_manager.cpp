#include "ecs_system_manager.hpp"

namespace ecs
{

	//------------------------------------------------------------------------------------------------------------------------
	csystem_manager::csystem_manager(flecs::world& w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	csystem_manager::~csystem_manager()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::system csystem_manager::system(stringview_t name) const
	{
		//- merely upcast the retrieved entity to system
		return flecs::system(world().lookup(name));
	}

} //- ecs