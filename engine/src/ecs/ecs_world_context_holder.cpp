#include "ecs_world_context_holder.hpp"
#include "ecs_world_manager.hpp"

namespace ecs
{
	namespace
	{
		constexpr auto C_AVRG_ENTITY_COMPONENT_COUNT = 8;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	iworld_context_holder::iworld_context_holder(cworld* w) :
		m_world(w)
	{
		CORE_ASSERT(m_world, "Invalid operation. Provided world is not valid!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	iworld_context_holder::~iworld_context_holder()
	{
		//- we are not responsible for memory
		m_world = nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld& iworld_context_holder::world()
	{
		return *m_world;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const cworld& iworld_context_holder::world() const
	{
		return *m_world;
	}

} //- ecs

RTTR_REGISTRATION
{
	using namespace ecs;

	rttr::cregistrator<iworld_context_holder, rttr::detail::no_default>("iworld_context_holder")
		.ctor<cworld*>(rttr::detail::constructor_policy_as_raw_ptr)
		;
}