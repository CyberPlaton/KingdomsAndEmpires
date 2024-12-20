#pragma once
#include "ecs_prefab.hpp"

namespace ecs
{
	//- Class responsible for prefab serialization, creation and destruction.
	//------------------------------------------------------------------------------------------------------------------------
	class cprefab_manager final : public iworld_context_holder
	{
	public:
		cprefab_manager(cworld* w);
		~cprefab_manager();

		cprefab& create_prefab();
		cprefab& create_prefab(stringview_t uuid);
		cprefab& create_prefab(const core::cuuid& uuid);
		cprefab& find(stringview_t name);
		void destroy(stringview_t name);

		cprefab& create(flecs::entity e);

		bool load(const core::cpath& path);
		void save(stringview_t name, const core::cpath& path);

	private:
		umap_t<unsigned, cprefab> m_prefabs;
	};

} //- ecs
