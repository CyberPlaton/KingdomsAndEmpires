#pragma once
#include <core.h>
#include "ecs_world_context_holder.hpp"
#include "ecs_component.hpp"

namespace ecs
{
	//- TODO: currently this manager is not functional and his purpose is not yet decided.
	//------------------------------------------------------------------------------------------------------------------------
	class ccomponent_manager : public iworld_context_holder
	{
	public:
		ccomponent_manager(flecs::world& w);

		void on_component_added(flecs::entity e, const string_t& component);
		void on_component_removed(flecs::entity e, const string_t& component);
		const vector_t<string_t> all(flecs::entity e) const;
		const vector_t<string_t>& components() const;

		//- If possible, avoid. Slow function
		bool has(const flecs::entity e, stringview_t component) const;

	private:
		vector_t<string_t> m_registered_components;
		umap_t<uint64_t, vector_t<string_t>> m_components;
	};

} //- ecs