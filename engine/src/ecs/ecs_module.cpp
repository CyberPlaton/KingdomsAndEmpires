#include "ecs_module.hpp"
#include "ecs_component.hpp"
#include "ecs_world_manager.hpp"
#include <plugin_logging.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	imodule::imodule(flecs::world& w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity imodule::module() const
	{
		return m_module;
	}

} //- ecs