#include "ecs_module_manager.hpp"
#include <plugin_logging.h>

namespace ecs
{
	namespace
	{

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager::cmodule_manager(ref_t<flecs::world> w) :
		iworld_context_holder(w)
	{
	}

} //- ecs