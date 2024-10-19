#pragma once
#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//- manages all entities for a given world. Note that managed world is not necessarily the current active one.
	//- here we can execute all kind of entity related actions within our world.
	//------------------------------------------------------------------------------------------------------------------------
	class centity_manager final : public iworld_context_holder
	{
	public:
		centity_manager(cworld* w);
		~centity_manager();

		flecs::entity create_entity();
		flecs::entity create_entity(stringview_t uuid);
		flecs::entity create_entity(const core::cuuid& uuid);

		flecs::entity entity(const core::cuuid& uuid) const;

		const vector_t<flecs::entity>& entities() const;

		bool is_prefab(flecs::entity e) const;

	private:
		vector_t<flecs::entity> m_entities;
	};

} //- ecs