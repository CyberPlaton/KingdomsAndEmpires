#pragma once
#include <config.hpp>
#include <flecs.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API centity_manager
	{
	public:
		centity_manager(const flecs::world& world);

	private:
		flecs::world& m_world;
	};

} //- ecs