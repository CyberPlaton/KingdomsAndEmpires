#include "ecs_world_context_holder.hpp"
#include "ecs_world_manager.hpp"

namespace ecs
{
	namespace
	{
		constexpr auto C_AVRG_ENTITY_COMPONENT_COUNT = 8;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	iworld_context_holder::iworld_context_holder(flecs::world& w) :
		m_world(&w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	iworld_context_holder::~iworld_context_holder()
	{
		//- we are not responsible for memory
		m_world = nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::world& iworld_context_holder::world()
	{
		ASSERT(m_world, "Invalid operation. World was null!");
		return *m_world;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const flecs::world& iworld_context_holder::world() const
	{
		ASSERT(m_world, "Invalid operation. World was null!");
		return *m_world;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vector_t<std::string> iworld_context_holder::components(flecs::entity e) const
	{
		vector_t<std::string> out; out.reserve(C_AVRG_ENTITY_COMPONENT_COUNT);

		e.each([&](flecs::id c)
			{
				vector_t<std::string> names;
				std::string name = c.str().c_str();
				core::string_utils::split(name, '.', names);

				//- ensure that id is a valid component
				if (auto comp_type = rttr::type::get_by_name(names.back());
					comp_type.is_valid() && comp_type.is_derived_from<ecs::icomponent>())
				{
					out.push_back(names.back());
				}
			});

		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool iworld_context_holder::is_prefab(flecs::entity e) const
	{
		return e.has(flecs::Prefab);
	}

} //- ecs

RTTR_REGISTRATION
{
	using namespace ecs;

	rttr::cregistrator<iworld_context_holder, rttr::detail::no_default>("iworld_context_holder")
		.ctor<rttr::detail::as_raw_pointer, flecs::world&>()
		;
}