#include "ecs_entity_manager.hpp"
#include "ecs_component.hpp"
#include <plugin_logging.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	centity_manager::centity_manager(flecs::world& w) :
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
		auto e = world().entity(uuid.data());

		//- emplace default components
		auto& id = *e.add<sidentifier>().get_mut<sidentifier>();

		id.m_uuid = string_t(uuid.data());
		id.m_self = e;
		id.m_name = uuid.data();

		return m_entities.emplace_back(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity centity_manager::entity(const core::cuuid& uuid) const
	{
		return world().lookup(uuid.string().c_str());
	}

	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<flecs::entity>& centity_manager::entities() const
	{
		return m_entities;
	}

} //- ecs