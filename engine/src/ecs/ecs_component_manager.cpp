#include "ecs_component_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	ccomponent_manager::ccomponent_manager(flecs::world& w) :
		iworld_context_holder(w)
	{
		//- gather all component types registered to RTTR
		log_trace("Register ecs components:");

		for (const auto type : rttr::type::get<icomponent>().get_derived_classes())
		{
			log_trace(fmt::format("\t'{}'", type.get_name().data()));

			m_registered_components.emplace_back(type.get_name().data());
		}
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	void ccomponent_manager::on_component_added(flecs::entity e, const string_t& component)
	{
		m_components[e.id()].emplace_back(component);
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	void ccomponent_manager::on_component_removed(flecs::entity e, const string_t& component)
	{
		auto& v = m_components[e.id()];
		auto it = algorithm::find_at(v.begin(), v.end(), component);

		if (it != v.end())
		{
			algorithm::erase_at(v, it);
		}
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<string_t> ccomponent_manager::all(flecs::entity e) const
	{
		if (const auto it = m_components.find(e.id()); it != m_components.end())
		{
			return m_components.at(e.id());
		}
		return {};
	}

	const vector_t<string_t>& ccomponent_manager::components() const
	{
		return m_registered_components;
	}

} //- ecs