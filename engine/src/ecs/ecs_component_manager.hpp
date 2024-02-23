#pragma once
#include <core.h>
#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class ccomponent_manager : public iworld_context_holder
	{
	public:
		ccomponent_manager(flecs::world& w);

		void on_component_added(flecs::entity e, const std::string& component);
		void on_component_removed(flecs::entity e, const std::string& component);
		const vector_t<std::string>& components(flecs::entity e) const;

	private:
		umap_t<unsigned, vector_t<std::string>> m_components;
	};

} //- ecs