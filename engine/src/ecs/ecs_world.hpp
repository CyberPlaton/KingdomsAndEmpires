#pragma once
#include <flecs.h>
#include "ecs_module_manager.hpp"
#include "ecs_system_manager.hpp"
#include "ecs_entity_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class cworld
	{
	public:
		cworld();
		~cworld();



	private:
		flecs::world m_world;
	};

} //- ecs