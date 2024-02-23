#include "ecs_component_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	ccomponent_manager::ccomponent_manager(flecs::world& w) :
		iworld_context_holder(w)
	{
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	void ccomponent_manager::on_component_added(flecs::entity e, const std::string& component)
	{
		auto h = algorithm::hash(e.name().c_str());
		
		m_components[h].emplace_back(component);
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	void ccomponent_manager::on_component_removed(flecs::entity e, const std::string& component)
	{
		auto h = algorithm::hash(e.name().c_str());
		
		algorithm::erase_first(m_components.at(h), component);
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<std::string>& ccomponent_manager::components(flecs::entity e) const
	{
		auto h = algorithm::hash(e.name().c_str());
		
		return m_components.at(h);
	}

} //- ecs