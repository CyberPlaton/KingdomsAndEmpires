#include "ecs_entity_manager.hpp"
#include "ecs_component.hpp"
#include "ecs_world.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	centity_manager::centity_manager(cworld* w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	centity_manager::~centity_manager()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity centity_manager::create_entity()
	{
		return create_entity(core::cuuid{});
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity centity_manager::create_entity(const core::cuuid& uuid)
	{
		return create_entity(uuid.string().c_str());
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity centity_manager::create_entity(stringview_t uuid)
	{
		//- create runtime ecs entity uniquely identifiable by uuid
		auto e = world().ecs().entity(uuid.data());

		CORE_ASSERT(e.is_valid(), "Invalid operation. Created entity is invalid!");

		//- emplace default components
		e.add<sidentifier>();

		auto* id = e.get_mut<sidentifier>();

		id->m_uuid = string_t(uuid.data());
		id->m_name = uuid.data();

		return m_entities.emplace_back(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity centity_manager::entity(const core::cuuid& uuid) const
	{
		return world().ecs().lookup(uuid.string().c_str());
	}

	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<flecs::entity>& centity_manager::entities() const
	{
		return m_entities;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool centity_manager::is_prefab(flecs::entity e) const
	{
		return e.has(flecs::Prefab);
	}

} //- ecs