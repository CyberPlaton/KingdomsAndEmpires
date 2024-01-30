#pragma once
#include "ecs_entity.hpp"

namespace ecs
{
	//- manages all entities for a given world. Note that managed world is not necessarily the current active one.
	//- here we can execute all kind of entity related actions within our world.
	//------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API centity_manager final
	{
	public:
		centity_manager(flecs::world& w);
		~centity_manager();

		centity& create_entity();
		centity& create_entity(stringview_t uuid);

		flecs::entity entity(const core::cuuid& uuid) const;

	private:
		flecs::world& m_managed_world;
		vector_t<centity> m_entities;

	private:
		inline flecs::world& w() { return m_managed_world; }
		inline const flecs::world& w() const { return m_managed_world; }
	};

} //- ecs