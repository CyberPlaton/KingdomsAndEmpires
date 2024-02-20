#include "ecs_entity_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	centity_manager::centity_manager(flecs::world& w) :
		m_managed_world(w)
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
		auto e = w().entity(uuid);

		return m_entities.emplace_back(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity centity_manager::entity(const core::cuuid& uuid) const
	{
		return w().lookup(uuid.string().c_str());
	}

	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<flecs::entity>& centity_manager::entities() const
	{
		return m_entities;
	}

} //- ecs