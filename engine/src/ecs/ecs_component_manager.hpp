#pragma once
#include <core.h>
#include "ecs_world_context_holder.hpp"
#include "ecs_component.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class ccomponent_manager : public iworld_context_holder
	{
	public:
		ccomponent_manager(flecs::world& w);

		void on_component_added(flecs::entity e, const std::string& component);
		void on_component_removed(flecs::entity e, const std::string& component);
		const vector_t<std::string>& all(flecs::entity e) const;
		const vector_t<std::string>& components() const;

	private:
		vector_t<std::string> m_registered_components;
		umap_t<uint64_t, vector_t<std::string>> m_components;
	};

} //- ecs