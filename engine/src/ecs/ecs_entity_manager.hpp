#pragma once
#include <flecs.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class centity_manager
	{
	public:
		centity_manager(const flecs::world& world);

	private:
		flecs::world& m_world;
	};

} //- ecs