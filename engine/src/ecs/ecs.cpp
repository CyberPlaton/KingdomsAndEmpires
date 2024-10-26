#include "ecs.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	ecs::cworld& world()
	{
		return ecs::cworld_manager::instance().active();
	}

} //- ecs