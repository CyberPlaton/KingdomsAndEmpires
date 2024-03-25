#pragma once
#include "ecs_prefab.hpp"

namespace ecs
{
	//- responsible for prefab serialization, creation and destruction.
	//------------------------------------------------------------------------------------------------------------------------
	class cprefab_manager final : public iworld_context_holder
	{
	public:
		cprefab_manager(flecs::world& w);
		~cprefab_manager();

		cprefab& create(const std::string& name);
		cprefab& find(const std::string& name);
		void destroy(const std::string& name);

		bool load(const core::cpath& path);
		void save(const std::string& name, const core::cpath& path);

	private:
		umap_t<unsigned, cprefab> m_prefabs;
	};

} //- ecs
