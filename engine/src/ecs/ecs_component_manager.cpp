#include "ecs_component_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	ccomponent_manager::ccomponent_manager(flecs::world& w) :
		iworld_context_holder(w)
	{
		//- gather all component types registered to RTTR
		for (auto type : rttr::type::get_types())
		{
			for (auto base : type.get_base_classes())
			{
				if (base == rttr::type::get<icomponent>())
				{
					m_registered_components.emplace_back(type.get_name().data());
					break;
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	void ccomponent_manager::on_component_added(flecs::entity e, const std::string& component)
	{
		m_components[e.id()].emplace_back(component);
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	void ccomponent_manager::on_component_removed(flecs::entity e, const std::string& component)
	{
		auto& v = m_components[e.id()];
		auto it = algorithm::find_at(v.begin(), v.end(), component);

		if (it != v.end())
		{
			algorithm::erase_at(v, it);
		}
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<std::string>& ccomponent_manager::all(flecs::entity e) const
	{
		auto h = algorithm::hash(e.name().c_str());
		
		return m_components.at(h);
	}

	const vector_t<std::string>& ccomponent_manager::components() const
	{
		return m_registered_components;
	}

} //- ecs