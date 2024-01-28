#pragma once
#include <core.h>
#include <flecs.h>
#include <config.hpp>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API iworld_context_holder
	{
	public:
		iworld_context_holder(ref_t<flecs::world> w);
		~iworld_context_holder() = default;

	private:
		ref_t<flecs::world> m_world;

	protected:
		flecs::world& world() { ASSERT(m_world, "World for context was not set before retrieving!"); return *(m_world.get()); }
		ref_t<flecs::world> world_ref() { ASSERT(m_world, "World for context was not set before retrieving!"); return m_world; }
	};

} //- ecs